#include "oasis_parser.h"
#include "oasis_layout.h"
#include "oasis_exception.h"
#include "oasis_util.h"
#include <iostream>
#include <sstream>
#include <math.h>

#include "thread_worker.h"

using namespace std;
using namespace oasis;

OasisParser::OasisParser() :
    m_openfp(0),
    m_cached_id(ORID_Max),
    m_use_cached_id(false),
    m_cellname_idmode(UNKNOWN),
    m_textstring_idmode(UNKNOWN),
    m_propstring_idmode(UNKNOWN),
    m_propname_idmode(UNKNOWN),
    m_xname_idmode(UNKNOWN),
    m_cellname_refnum(0),
    m_textstring_refnum(0),
    m_propname_refnum(0),
    m_propstring_refnum(0),
    m_xname_refnum(0)
{
    m_offsets_at_end = true;
    m_cellprop_uselayers = true;
    m_parse_mode = PARSE;
}

OasisParser::~OasisParser()
{
    if(m_openfp)
    {
        fclose(m_openfp);
    }
}


void OasisParser::open_file(string filename)
{
    m_openfp = fopen(filename.c_str(), "rb");
    if(m_openfp == NULL)
    {
        OAS_DBG << "open file " << filename << " failed" << OAS_ENDL;
        throw OasisException("open file failed");
    }
    m_filename = filename;
}

void OasisParser::set_file_offset(uint64 offset, uint64 hint_size)
{
    m_reader.init(m_openfp, offset);
    m_reader.reset_buf_size(hint_size);
}

void OasisParser::parse_file(OasisLayout* layout)
{
    OAS_INFO << "begin parse file" << OAS_ENDL;

    oas_assert(layout != NULL);
    m_layout = layout;
    m_layout->cleanup();
    m_reader.init(m_openfp);

    parse_magic();
    parse_start();

    if (m_parse_option.parse_over())
    {
        parse_all();
        parse_end();
        return;
    }

    m_layout->open_file(true);
    m_layout->writer().write_magic();
    m_layout->write_start(m_layout->writer());

    m_layout->compress_geometry(m_parse_option.compress_geometry());

    if (m_parse_option.thread_count() > 0)
    {
        m_parse_option.parse_over(true);
        parse_all();

        m_parse_option.parse_over(false);
        m_layout->multiple_thread_write(true);
        parse_all_mthread();
    }
    else
    {
        parse_all();
    }

    m_layout->unload();
    m_layout->flush_writer(m_layout->writer());
    m_layout->resolve_alias();

    m_layout->collect_top_cells();
    m_layout->compute_deep_information();
    m_layout->unload(true);

    parse_end();
    m_layout->write_tables(m_layout->writer());
    m_layout->write_end(m_layout->writer());

    OAS_INFO << "end parse file" << OAS_ENDL;
}

void OasisParser::parse_cell_concurrent(int64 info_index)
{
    OasisCellInfo &info = m_layout->get_cell_info(info_index);
    OasisCell &cell = m_layout->get_cell(info.get_cell_index());
    OasisParser parser;
    parser.set_parse_option(m_parse_option);
    parser.open_file(m_filename);
    parser.set_file_offset(cell.get_offset(), cell.get_hint_datasize());
    parser.set_layout(m_layout);
    parser.parse_cell_second_pass(info.get_cell_index());
}

void OasisParser::parse_cell_second_pass(int64 cell_index)
{
    m_parse_mode = SECOND_PASS;
    OasisRecordId rid = read_record_id();
    OAS_DBG << "Record " << oasis_record_id_name(rid) << OAS_ENDL;
    parse_cell(rid, cell_index);
}

void OasisParser::parse_magic()
{
    uint8 magicbuf[sizeof(OasisMagic)];
    m_reader.read_bytes(magicbuf, sizeof(OasisMagic));
    if(memcmp(magicbuf, OasisMagic, sizeof(OasisMagic)) != 0)
    {
        throw OasisException("verify magic failed");
    }
}

void OasisParser::parse_start()
{
    OasisRecordId rid = read_record_id();
    OAS_DBG << "Record " << oasis_record_id_name(rid) << OAS_ENDL;
    if(rid != ORID_Start)
    {
        throw OasisException("start id wrong ");
    }
    OasisString version;
    m_reader.read_string(version);
    if(version.value() != "1.0")
    {
        OAS_DBG << "version wrong " << version.value() << OAS_ENDL;
        throw OasisException("version wrong");
    }
    OasisReal resolution;
    m_reader.read_real(resolution);
    if(resolution.value() < 1e-6)
    {
        OAS_DBG << "resolution wrong " << resolution.value() << OAS_ENDL;
        throw OasisException("resolution wrong");
    }

    float64 dbu = 1.0 / resolution.value();

    OAS_DBG << "version " << version.value() << OAS_ENDL;
    OAS_DBG << "resolution " << resolution.value() << OAS_ENDL;
    OAS_DBG << "dbu " << dbu << OAS_ENDL;

    m_layout->set_dbu(dbu);

    m_offsets_at_end = m_reader.read_uint();
    OAS_DBG << "m_offsets at end " << m_offsets_at_end << OAS_ENDL;
    if(!m_offsets_at_end)
    {
        read_table_offsets();
    }

    int64 offset = m_reader.get_data_offset();

    if(m_offsets_at_end)
    {
        m_reader.seek(-256, true);
        cache_id(false);
        OasisRecordId rid = read_record_id();
        if(rid != ORID_End)
        {
            throw OasisException("expect end record");
        }
        read_table_offsets();
    }
    import_tables();

    m_reader.seek(offset);
    cache_id(false);
    read_file_properties();
}

void OasisParser::read_table_offsets()
{
    m_layout->m_cellname_flag = m_reader.read_uint();
    m_layout->m_cellname_offset = m_reader.read_uint();
    m_layout->m_textstring_flag = m_reader.read_uint();
    m_layout->m_textstring_offset = m_reader.read_uint();
    m_layout->m_propname_flag = m_reader.read_uint();
    m_layout->m_propname_offset = m_reader.read_uint();
    m_layout->m_propstring_flag = m_reader.read_uint();
    m_layout->m_propstring_offset = m_reader.read_uint();
    m_layout->m_layername_flag = m_reader.read_uint();
    m_layout->m_layername_offset = m_reader.read_uint();
    m_layout->m_xname_flag = m_reader.read_uint();
    m_layout->m_xname_offset = m_reader.read_uint();

    OAS_DBG << "cellname flag " << m_layout->m_cellname_flag
            << " offset " << m_layout->m_cellname_offset << OAS_ENDL;
    OAS_DBG << "textstring flag " << m_layout->m_textstring_flag
            << " offset " << m_layout->m_textstring_offset << OAS_ENDL;
    OAS_DBG << "propname flag " << m_layout->m_propname_flag
            << " offset " << m_layout->m_propname_offset << OAS_ENDL;
    OAS_DBG << "propstring flag " << m_layout->m_propstring_flag
            << " offset " << m_layout->m_propstring_offset << OAS_ENDL;
    OAS_DBG << "layername flag " << m_layout->m_layername_flag
            << " offset " << m_layout->m_layername_offset << OAS_ENDL;
    OAS_DBG << "xname flag " << m_layout->m_xname_flag
            << " offset " << m_layout->m_xname_offset << OAS_ENDL;
}

void OasisParser::parse_end()
{
    OasisRecordId rid = read_record_id();
    if(rid != ORID_End)
    {
        throw OasisException("expect end record");
    }

    int64 offset = m_reader.get_data_offset();
    if(m_offsets_at_end)
    {
        read_table_offsets();
    }
    int64 tablesize = m_reader.get_data_offset() - offset;
    OAS_DBG << "table size " << tablesize << OAS_ENDL;
    if(1)
    {
        m_reader.skip_bytes(255 - tablesize);
    }
    else
    {
        OasisString pad;
        m_reader.read_string(pad);
        uint64 validate_scheme = m_reader.read_uint();
        if(validate_scheme != 0)
        {
            m_reader.skip_bytes(4);
        }
    }

    int64 dataend = m_reader.get_data_offset();
    int64 filesize = m_reader.get_file_size();
    if(filesize != dataend)
    {
        OAS_DBG << "file size " << filesize << " dataend " << dataend << OAS_ENDL;
        throw OasisException("filesize error");
    }
}

void OasisParser::parse_all()
{
    OAS_INFO << "begin parse all cell" << OAS_ENDL;
    m_modal_variables.reset();

    m_cellname_idmode = UNKNOWN;
    m_textstring_idmode = UNKNOWN;
    m_propname_idmode = UNKNOWN;
    m_propstring_idmode = UNKNOWN;
    m_xname_idmode = UNKNOWN;
    m_cellname_refnum = 0;
    m_textstring_refnum = 0;
    m_propname_refnum = 0;
    m_propstring_refnum = 0;
    m_xname_refnum = 0;

    bool done = false;
    while(!done)
    {
        OasisRecordId rid = read_record_id();
        OAS_DBG << "Record " << oasis_record_id_name(rid) << OAS_ENDL;
        switch(rid)
        {
            case ORID_Pad:
                break;
            case ORID_End:
                done = true;
                cache_id();
                break;
            case ORID_Cellname:
            case ORID_CellnameRef:
                parse_cell_name(rid, m_layout->cellname_strictmode());
                break;
            case ORID_Textstring:
            case ORID_TextstringRef:
                parse_text_string(rid, m_layout->textstring_strictmode());
                break;
            case ORID_Propname:
            case ORID_PropnameRef:
                parse_prop_name(rid, m_layout->propname_strictmode());
                break;
            case ORID_Propstring:
            case ORID_PropstringRef:
                parse_prop_string(rid, m_layout->propstring_strictmode());
                break;
            case ORID_Layername:
            case ORID_LayernameText:
                parse_layer_name(rid, m_layout->layername_strictmode());
                break;
            case ORID_Cell:
            case ORID_CellRef:
            {
                int64 cell_index = m_layout->create_cell();
                OasisCell& cell = m_layout->get_cell(cell_index);
                int64 offset = m_reader.get_data_offset() - 1;
                cell.set_offset(offset);
                parse_cell(rid, cell_index);
                cell.set_hint_datasize(m_reader.get_data_offset() - offset);
                break;
            }
            case ORID_Property:
                read_properties(rid);
            case ORID_PropertyLast:
                OAS_DBG << "skip unassociated property in parse all" << OAS_ENDL;
                break;
            case ORID_XName:
            case ORID_XNameRef:
                parse_x_name(rid, m_layout->xname_strictmode());
                break;
            case ORID_Cblock:
                enter_cblock(rid);
                break;
            default:
                OAS_ERR << "invalid rid " << (int)rid << OAS_ENDL;
                throw OasisException("invalid rid");
                break;
        }
    }
    OAS_INFO << "end parse all cell" << OAS_ENDL;
}

void OasisParser::parse_cell(const OasisRecordId& rid, int64 cell_index)
{
    oas_assert(rid == ORID_Cell || rid == ORID_CellRef);

    OAS_DBG << "parse mode " << static_cast<uint32>(m_parse_mode) << OAS_ENDL;

    int64 current_info_index = -1;
    if(rid == ORID_CellRef)
    {
        if(m_parse_mode == RELOAD)
        {
            current_info_index = m_reader.read_uint();
        }
        else
        {
            uint64 ref_num = m_reader.read_uint();
            if(m_parse_mode == PARSE)
            {
                if(m_layout->m_defined_cells_by_ref.find(ref_num) != m_layout->m_defined_cells_by_ref.end())
                {
                    OAS_ERR << "redefine cell " << ref_num << OAS_ENDL;
                    throw OasisException("redefine cell");
                }
                m_layout->m_defined_cells_by_ref.insert(ref_num);
            }

            if (m_parse_mode == SECOND_PASS)
            {
                current_info_index = m_layout->m_cellref2index[ref_num];
            }
            else
            {
                std::map<uint64, int64>::iterator c = m_layout->m_cellref2index.find(ref_num);
                int64 index = -1;
                if(c == m_layout->m_cellref2index.end())
                {
                    oas_assert(!m_layout->cellname_strictmode());

                    index = m_layout->create_cell_info(ref_num, "");
                    m_layout->m_cellref2index[ref_num] = index;
                    OAS_DBG << "create info by ref " << ref_num << " index " << index << OAS_ENDL;
                }
                else
                {
                    index = c->second;
                    OAS_DBG << "find cellref " << ref_num << " index " << index << OAS_ENDL;
                }
                OasisCellInfo& info = m_layout->get_cell_info(index);
                info.set_cell_index(cell_index);
                current_info_index = index;
            }
        }
    }
    else
    {
        oas_assert(!m_layout->cellname_strictmode());

        OasisString cell_name;
        m_reader.read_string(cell_name);
        oas_assert(cell_name.is_nstring());

        if ((m_parse_mode == PARSE || m_parse_mode == SECOND_PASS) && is_generated_cellname(cell_name.value()))
        {
            std::string new_value = rename_generated_prefix(cell_name.value());
            cell_name.set_value(new_value);
        }

        if (m_parse_mode == PARSE)
        {
            if(m_layout->m_defined_cells_by_name.find(cell_name.value()) != m_layout->m_defined_cells_by_name.end())
            {
                OAS_ERR << "redefine cell " << cell_name.value() << OAS_ENDL;
                throw OasisException("redefine cell");
            }
            m_layout->m_defined_cells_by_name.insert(cell_name.value());
        }

        if (m_parse_mode == SECOND_PASS)
        {
            current_info_index = m_layout->m_cellname2index[cell_name.value()];
        }
        else
        {
            std::map<std::string, int64>::iterator c = m_layout->m_cellname2index.find(cell_name.value());
            int64 index = -1;
            if (c == m_layout->m_cellname2index.end())
            {
                oas_assert(m_parse_mode == PARSE);
                index = m_layout->create_cell_info(-1, cell_name.value());
                m_layout->m_cellname2index[cell_name.value()] = index;
                OAS_DBG << "create cell info by name " << cell_name.value() << " index " << index << OAS_ENDL;
            }
            else
            {
                index = c->second;
                OAS_DBG << "find cellname " << cell_name.value() << " index " << index << OAS_ENDL;
            }
            OasisCellInfo &info = m_layout->get_cell_info(index);
            info.set_cell_index(cell_index);
            current_info_index = index;
        }
    }

    OAS_DBG << "current cell index " << cell_index << OAS_ENDL;
    OAS_DBG << "current info index " << current_info_index << OAS_ENDL;

    OasisCell &cell = m_layout->get_cell(cell_index);
    if(m_parse_mode == PARSE)
    {
        cell.set_infoindex(current_info_index);
    }
    else
    {
        oas_assert(cell.get_infoindex() == current_info_index);
    }

    m_modal_variables.reset();
    read_cell_properties();

    bool done = false;
    while(!done)
    {
        OasisRecordId rid = read_record_id();
        OAS_DBG << "Record " << oasis_record_id_name(rid) << " in parse cell" << OAS_ENDL;
        switch(rid)
        {
            case ORID_Pad:
                break;
            case ORID_XYAbsolute:
                m_modal_variables.m_xymode = ModalVariable::XYAbsolute;
                break;
            case ORID_XYRelative:
                m_modal_variables.m_xymode = ModalVariable::XYRelative;
                break;
            case ORID_Placement:
            case ORID_PlacementTrans:
                read_placement(rid, cell);
                break;
            case ORID_Text:
                read_text(rid, cell);
                break;
            case ORID_Rectangle:
                read_rectangle(rid, cell);
                break;
            case ORID_Polygon:
                read_polygon(rid, cell);
                break;
            case ORID_Path:
                read_path(rid, cell);
                break;
            case ORID_Trapezoid:
            case ORID_TrapezoidA:
            case ORID_TrapezoidB:
                read_trapezoid(rid, cell);
                break;
            case ORID_CTrapezoid:
                read_c_trapezoid(rid, cell);
                break;
            case ORID_Circle:
                read_circle(rid, cell);
                break;
            case ORID_Property:
                read_properties(rid);
            case ORID_PropertyLast:
                oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropertyBit));
                oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropStandardBit));
                oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropnameBit));
                oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropvaluesBit));
                OAS_DBG << "skip property" << OAS_ENDL;
                break;
            case ORID_XElement:
                read_x_element(rid, cell);
                break;
            case ORID_XGeometry:
                read_x_geometry(rid, cell);
                break;
            case ORID_Cblock:
                enter_cblock(rid);
                break;
            default:
                done = true;
                cache_id();
                break;
        }
    }

    if (m_parse_option.parse_over())
    {
        return;
    }

    if(m_parse_mode == PARSE || m_parse_mode == SECOND_PASS)
    {
        if(m_parse_option.do_partition())
        {
            cell.partition(*m_layout);
        }
        cell.try_compute_deep_information(*m_layout);
        cell.unload(*m_layout);
    }
}

OasisRecordId OasisParser::read_record_id()
{
    if(m_use_cached_id)
    {
        m_use_cached_id = false;
    }
    else
    {
        uint8 b = m_reader.read_byte();
        oas_assert(b < ORID_Max);
        m_cached_id = static_cast<OasisRecordId>(b);
    }
    return m_cached_id;
}

void OasisParser::read_placement(const OasisRecordId& rid, OasisCell& cell)
{
    using namespace PlacementInfoByte;
    oas_assert(rid == ORID_Placement || rid == ORID_PlacementTrans);

    uint8 info_byte = m_reader.read_byte();

    if(info_byte & HasExplicitCell)
    {
        if(info_byte & HasRefnum)
        {
            if(m_parse_mode == RELOAD)
            {
                int64 index = m_reader.read_uint();
                m_modal_variables.m_placement_cell = index;
                m_modal_variables.define_bit(ModalVariable::PlacementCellBit);
            }
            else
            {
                uint64 ref_num = m_reader.read_uint();
                if (m_parse_mode != SECOND_PASS)
                {
                    std::map<uint64, int64>::iterator c = m_layout->m_cellref2index.find(ref_num);
                    int64 index = -1;
                    if (c == m_layout->m_cellref2index.end())
                    {
                        oas_assert(!m_layout->cellname_strictmode());

                        index = m_layout->create_cell_info(ref_num, "");
                        m_layout->m_cellref2index[ref_num] = index;
                        OAS_DBG << "placement create cellref " << ref_num << " index " << index << OAS_ENDL;
                    }
                    else
                    {
                        index = c->second;
                        OAS_DBG << "placement find cellref " << ref_num << " index " << index << OAS_ENDL;
                    }
                    m_modal_variables.m_placement_cell = index;
                    m_modal_variables.define_bit(ModalVariable::PlacementCellBit);
                    OasisCellInfo &info = m_layout->get_cell_info(index);
                    info.set_referenced();

                    if (m_parse_mode == PARSE)
                    {
                        m_layout->m_info2children[cell.get_infoindex()].insert(index);
                        m_layout->m_info2parents[index].insert(cell.get_infoindex());
                    }
                }
                else
                {
                    m_modal_variables.m_placement_cell = m_layout->m_cellref2index.at(ref_num);
                    m_modal_variables.define_bit(ModalVariable::PlacementCellBit);
                }
            }
        }
        else
        {
            oas_assert(!m_layout->cellname_strictmode());

            OasisString cellname;
            m_reader.read_string(cellname);

            if ((m_parse_mode == PARSE || m_parse_mode == SECOND_PASS) && is_generated_cellname(cellname.value()))
            {
                std::string new_value = rename_generated_prefix(cellname.value());
                cellname.set_value(new_value);
            }

            if (m_parse_mode != SECOND_PASS)
            {
                std::map<std::string, int64>::iterator c = m_layout->m_cellname2index.find(cellname.value());
                int64 index = -1;
                if (c == m_layout->m_cellname2index.end())
                {
                    index = m_layout->create_cell_info(-1, cellname.value());
                    m_layout->m_cellname2index[cellname.value()] = index;
                    OAS_DBG << "placement create cellname " << cellname.value() << " index " << index << OAS_ENDL;
                }
                else
                {
                    index = c->second;
                    OAS_DBG << "placement find cellname " << cellname.value() << " index " << index << OAS_ENDL;
                }
                m_modal_variables.m_placement_cell = index;
                m_modal_variables.define_bit(ModalVariable::PlacementCellBit);
                OasisCellInfo &info = m_layout->get_cell_info(index);
                info.set_referenced();

                if (m_parse_mode == PARSE)
                {
                    m_layout->m_info2children[cell.get_infoindex()].insert(index);
                    m_layout->m_info2parents[index].insert(cell.get_infoindex());
                }
            }
            else
            {
                m_modal_variables.m_placement_cell = m_layout->m_cellname2index.at(cellname.value());
                m_modal_variables.define_bit(ModalVariable::PlacementCellBit);
            }
        }
    }

    OasisReal mag;
    bool mag_set = false;

    OasisReal angle_deg;
    bool angle_deg_set = false;
    int angle = Angle0;

    if(rid == ORID_PlacementTrans)
    {
        if(info_byte & HasMag)
        {
            m_reader.read_real(mag);
            if(fabs(mag.value() - 1.0) > 1e-6)
            {
                mag_set = true;
            }
        }

        if(info_byte & HasAngle)
        {
            m_reader.read_real(angle_deg);
            if(fabs(angle_deg.value()) > 1e-6)
            {
                angle_deg_set = true;
            }
        }
    }
    else
    {
        angle = ((info_byte & AngleMask) >> 1);
    }

    bool mirror = (info_byte & HasMirror) != 0;

    if(info_byte & HasX)
    {
        if(m_modal_variables.m_xymode == ModalVariable::XYAbsolute)
        {
            m_modal_variables.m_placement_x = m_reader.read_int();
        }
        else
        {
            m_modal_variables.m_placement_x = m_reader.read_int() + m_modal_variables.m_placement_x;
        }
        m_modal_variables.define_bit(ModalVariable::PlacementXBit);
    }

    if(info_byte & HasY)
    {
        if(m_modal_variables.m_xymode == ModalVariable::XYAbsolute)
        {
            m_modal_variables.m_placement_y = m_reader.read_int();
        }
        else
        {
            m_modal_variables.m_placement_y = m_reader.read_int() + m_modal_variables.m_placement_y;
        }
        m_modal_variables.define_bit(ModalVariable::PlacementYBit);
    }

    if (info_byte & HasRepetition)
    {
        m_reader.read_repetition(m_modal_variables.m_repetition);
        m_modal_variables.define_bit(ModalVariable::RepetitionBit);
    }

    read_element_properties();

    if (m_parse_option.parse_over())
    {
        return;
    }

    oas_assert(m_modal_variables.defined_bit(ModalVariable::PlacementXBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::PlacementYBit));
    Point disp(m_modal_variables.m_placement_x, m_modal_variables.m_placement_y);

    OasisTrans trans;
    if(rid == ORID_PlacementTrans)
    {
        if(mag_set || angle_deg_set)
        {
            trans = OasisTrans(
                            mirror,
                            angle_deg_set ? angle_deg.value() : 0.0,
                            mag_set ? mag.value() : 1.0,
                            disp
                            );
        }
        else
        {
            trans = OasisTrans(mirror, angle, disp);
        }
    }
    else
    {
        trans = OasisTrans(mirror, angle, disp);
    }

    oas_assert(m_modal_variables.defined_bit(ModalVariable::PlacementCellBit));
    int64 infoindex = m_modal_variables.m_placement_cell;

    if(info_byte & HasRepetition)
    {
        OasisRepetitionBuilder builder(m_parse_option.do_partition());
        std::vector<OasisRepetition> repetitions;
        std::vector<Point> offsets;
        if(builder.partition(m_modal_variables.m_repetition, m_layout->repetition_dimension_limit(), repetitions, offsets))
        {
            for(uint32 i = 0; i < repetitions.size(); ++i)
            {
                PlacementElement* e = new PlacementElement;
                OasisRepetition& repetition = repetitions[i];
                Point& offset = offsets[i];
                e->create(
                    infoindex,
                    trans.shifted(offset),
                    (repetition.valid() ? &repetition : NULL)
                    );
                cell.add_instance(e);
            }
        }
        else
        {
            PlacementElement* e = new PlacementElement;
            e->create(
                infoindex,
                trans,
                &m_modal_variables.m_repetition
                );
            cell.add_instance(e);
        }
    }
    else
    {
        PlacementElement* e = new PlacementElement;
        e->create(
            infoindex,
            trans
            );
        cell.add_instance(e);
    }

}

void OasisParser::read_text(const OasisRecordId& rid, OasisCell& cell)
{
    using namespace TextInfoByte;
    oas_assert(rid == ORID_Text);

    uint8 info_byte = m_reader.read_byte();

    if(info_byte & HasExplicitText)
    {
        int64 index;
        if(info_byte & HasRefnum)
        {
            uint64 ref_num = m_reader.read_uint();
            if(m_layout->m_textref2index.find(ref_num) == m_layout->m_textref2index.end())
            {
                oas_assert(!m_layout->textstring_strictmode());

                index = m_layout->m_textstrings.size();
                OasisTextString ots;
                ots.set_index(index);
                ots.set_refnum(ref_num);
                m_layout->m_textstrings.push_back(ots);
                m_layout->m_textref2index[ref_num] = index;
            }
            else
            {
                index = m_layout->m_textref2index[ref_num];
                oas_assert((uint64)index < m_layout->m_textstrings.size());
            }
        }
        else
        {
            oas_assert(!m_layout->textstring_strictmode());

            OasisString text_string;
            m_reader.read_string(text_string);
            if(m_layout->m_textstring2index.find(text_string.value()) == m_layout->m_textstring2index.end())
            {
                index = m_layout->m_textstrings.size();
                OasisTextString ots;
                ots.set_index(index);
                ots.set_name(text_string);
                m_layout->m_textstrings.push_back(ots);
                m_layout->m_textstring2index[text_string.value()] = index;
            }
            else
            {
                index = m_layout->m_textstring2index[text_string.value()];
                oas_assert((uint64)index < m_layout->m_textstrings.size());
            }
        }

        m_modal_variables.m_textstring = m_layout->m_textstrings[index];
        m_modal_variables.define_bit(ModalVariable::TextStringBit);
    }

    if(info_byte & HasTextlayer)
    {
        m_modal_variables.m_textlayer = m_reader.read_uint();
        m_modal_variables.define_bit(ModalVariable::TextLayerBit);
    }

    if(info_byte & HasTexttype)
    {
        m_modal_variables.m_texttype = m_reader.read_uint();
        m_modal_variables.define_bit(ModalVariable::TextTypeBit);
    }

    if(info_byte & HasX)
    {
        if(m_modal_variables.m_xymode == ModalVariable::XYAbsolute)
        {
            m_modal_variables.m_text_x = m_reader.read_int();
        }
        else
        {
            m_modal_variables.m_text_x = m_reader.read_int() + m_modal_variables.m_text_x;
        }
        m_modal_variables.define_bit(ModalVariable::TextXBit);
    }

    if(info_byte & HasY)
    {
        if(m_modal_variables.m_xymode == ModalVariable::XYAbsolute)
        {
            m_modal_variables.m_text_y = m_reader.read_int();
        }
        else
        {
            m_modal_variables.m_text_y = m_reader.read_int() + m_modal_variables.m_text_y;
        }
        m_modal_variables.define_bit(ModalVariable::TextYBit);
    }

    if (info_byte & HasRepetition)
    {
        m_reader.read_repetition(m_modal_variables.m_repetition);
        m_modal_variables.define_bit(ModalVariable::RepetitionBit);
    }

    read_element_properties();

    if (m_parse_option.parse_over())
    {
        return;
    }

    oas_assert(m_modal_variables.defined_bit(ModalVariable::TextXBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::TextYBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::TextStringBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::TextLayerBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::TextTypeBit));

    LDType ld = m_parse_option.map_layer(m_modal_variables.m_textlayer, m_modal_variables.m_texttype);
    if(info_byte & HasRepetition)
    {
        if(m_parse_option.parse_text() && m_parse_option.include_layer(m_modal_variables.m_textlayer, m_modal_variables.m_texttype))
        {
            OasisRepetitionBuilder builder(m_parse_option.do_partition());
            std::vector<OasisRepetition> repetitions;
            std::vector<Point> offsets;
            if(builder.partition(m_modal_variables.m_repetition, m_layout->repetition_dimension_limit(), repetitions, offsets))
            {
                for(uint32 i = 0; i < repetitions.size(); ++i)
                {
                    TextElement* t = new TextElement;
                    OasisRepetition& repetition = repetitions[i];
                    Point& offset = offsets[i];
                    t->create(
                        m_modal_variables.m_text_x + offset.x(),
                        m_modal_variables.m_text_y + offset.y(),
                        m_modal_variables.m_textstring,
                        ld.layer,
                        ld.datatype,
                        (repetition.valid() ? &repetition : NULL)
                        );
                    cell.add_text(t);
                }
            }
            else
            {
                TextElement* t = new TextElement;
                t->create(
                    m_modal_variables.m_text_x,
                    m_modal_variables.m_text_y,
                    m_modal_variables.m_textstring,
                    ld.layer,
                    ld.datatype,
                    &m_modal_variables.m_repetition
                    );
                cell.add_text(t);
            }
        }
    }
    else
    {
        if(m_parse_option.parse_text() && m_parse_option.include_layer(m_modal_variables.m_textlayer, m_modal_variables.m_texttype))
        {
            TextElement* t = new TextElement;
            t->create(
                m_modal_variables.m_text_x,
                m_modal_variables.m_text_y,
                m_modal_variables.m_textstring,
                ld.layer,
                ld.datatype
                );
            cell.add_text(t);
        }
    }
}

void OasisParser::read_rectangle(const OasisRecordId& rid, OasisCell& cell)
{
    using namespace RectangleInfoByte;
    oas_assert(rid == ORID_Rectangle);

    uint8 info_byte = m_reader.read_byte();

    if(info_byte & HasLayer)
    {
        m_modal_variables.m_layer = m_reader.read_uint();
        m_modal_variables.define_bit(ModalVariable::LayerBit);
    }

    if(info_byte & HasDatatype)
    {
        m_modal_variables.m_datatype = m_reader.read_uint();
        m_modal_variables.define_bit(ModalVariable::DataTypeBit);
    }

    if(info_byte & HasWidth)
    {
        m_modal_variables.m_geometry_w = m_reader.read_uint();
        m_modal_variables.define_bit(ModalVariable::GeometryWBit);
    }

    if(info_byte & IsSquare)
    {
        m_modal_variables.m_geometry_h = m_modal_variables.m_geometry_w;
        oas_assert((info_byte & HasHeight) == 0);
        m_modal_variables.define_bit(ModalVariable::GeometryHBit);
    }
    else if(info_byte & HasHeight)
    {
        m_modal_variables.m_geometry_h = m_reader.read_uint();
        m_modal_variables.define_bit(ModalVariable::GeometryHBit);
    }

    if(info_byte & HasX)
    {
        if(m_modal_variables.m_xymode == ModalVariable::XYAbsolute)
        {
            m_modal_variables.m_geometry_x = m_reader.read_int();
        }
        else
        {
            m_modal_variables.m_geometry_x = m_reader.read_int() + m_modal_variables.m_geometry_x;
        }
        m_modal_variables.define_bit(ModalVariable::GeometryXBit);
    }

    if(info_byte & HasY)
    {
        if(m_modal_variables.m_xymode == ModalVariable::XYAbsolute)
        {
            m_modal_variables.m_geometry_y = m_reader.read_int();
        }
        else
        {
            m_modal_variables.m_geometry_y = m_reader.read_int() + m_modal_variables.m_geometry_y;
        }
        m_modal_variables.define_bit(ModalVariable::GeometryYBit);
    }

    if (info_byte & HasRepetition)
    {
        m_reader.read_repetition(m_modal_variables.m_repetition);
        m_modal_variables.define_bit(ModalVariable::RepetitionBit);
    }

    read_element_properties();

    if (m_parse_option.parse_over())
    {
        return;
    }

    oas_assert(m_modal_variables.defined_bit(ModalVariable::GeometryXBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::GeometryYBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::LayerBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::DataTypeBit));

    oas_assert(m_modal_variables.defined_bit(ModalVariable::GeometryWBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::GeometryHBit));

    LDType ld = m_parse_option.map_layer(m_modal_variables.m_layer, m_modal_variables.m_datatype);
    if(info_byte & HasRepetition)
    {
        if(m_parse_option.include_layer(m_modal_variables.m_layer, m_modal_variables.m_datatype))
        {
            OasisRepetitionBuilder builder(m_parse_option.do_partition());
            std::vector<OasisRepetition> repetitions;
            std::vector<Point> offsets;
            if(builder.partition(m_modal_variables.m_repetition, m_layout->repetition_dimension_limit(), repetitions, offsets))
            {
                for(uint32 i = 0; i < repetitions.size(); ++i)
                {
                    RectangleElement* e = new RectangleElement;
                    OasisRepetition& repetition = repetitions[i];
                    Point& offset = offsets[i];
                    e->create(
                        m_modal_variables.m_geometry_x + offset.x(),
                        m_modal_variables.m_geometry_y + offset.y(),
                        m_modal_variables.m_geometry_w,
                        m_modal_variables.m_geometry_h,
                        ld.layer,
                        ld.datatype,
                        (repetition.valid() ? &repetition : NULL)
                        );
                    cell.add_geometry(e);
                }
            }
            else
            {
                RectangleElement* e = new RectangleElement;
                e->create(
                    m_modal_variables.m_geometry_x,
                    m_modal_variables.m_geometry_y,
                    m_modal_variables.m_geometry_w,
                    m_modal_variables.m_geometry_h,
                    ld.layer,
                    ld.datatype,
                    &m_modal_variables.m_repetition
                    );
                cell.add_geometry(e);
            }
        }
    }
    else
    {
        if(m_parse_option.include_layer(m_modal_variables.m_layer, m_modal_variables.m_datatype))
        {
            RectangleElement* e = new RectangleElement;
            e->create(
                m_modal_variables.m_geometry_x,
                m_modal_variables.m_geometry_y,
                m_modal_variables.m_geometry_w,
                m_modal_variables.m_geometry_h,
                ld.layer,
                ld.datatype
                );
            cell.add_geometry(e);
        }
    }

}

void OasisParser::read_polygon(const OasisRecordId& rid, OasisCell& cell)
{
    using namespace PolygonInfoByte;
    oas_assert(rid == ORID_Polygon);

    uint8 info_byte = m_reader.read_byte();

    if(info_byte & HasLayer)
    {
        m_modal_variables.m_layer = m_reader.read_uint();
        m_modal_variables.define_bit(ModalVariable::LayerBit);
    }

    if(info_byte & HasDatatype)
    {
        m_modal_variables.m_datatype = m_reader.read_uint();
        m_modal_variables.define_bit(ModalVariable::DataTypeBit);
    }

    if(info_byte & HasPointList)
    {
        m_reader.read_point_list(m_modal_variables.m_polygon_pointlist);
        m_modal_variables.define_bit(ModalVariable::PolygonPointListBit);
    }

    if(info_byte & HasX)
    {
        if(m_modal_variables.m_xymode == ModalVariable::XYAbsolute)
        {
            m_modal_variables.m_geometry_x = m_reader.read_int();
        }
        else
        {
            m_modal_variables.m_geometry_x = m_reader.read_int() + m_modal_variables.m_geometry_x;
        }
        m_modal_variables.define_bit(ModalVariable::GeometryXBit);
    }

    if(info_byte & HasY)
    {
        if(m_modal_variables.m_xymode == ModalVariable::XYAbsolute)
        {
            m_modal_variables.m_geometry_y  = m_reader.read_int();
        }
        else
        {
            m_modal_variables.m_geometry_y = m_reader.read_int() + m_modal_variables.m_geometry_y;
        }
        m_modal_variables.define_bit(ModalVariable::GeometryYBit);
    }

    if (info_byte & HasRepetition)
    {
        m_reader.read_repetition(m_modal_variables.m_repetition);
        m_modal_variables.define_bit(ModalVariable::RepetitionBit);
    }

    read_element_properties();

    if (m_parse_option.parse_over())
    {
        return;
    }

    oas_assert(m_modal_variables.defined_bit(ModalVariable::GeometryXBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::GeometryYBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::PolygonPointListBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::LayerBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::DataTypeBit));

    LDType ld = m_parse_option.map_layer(m_modal_variables.m_layer, m_modal_variables.m_datatype);
    if(info_byte & HasRepetition)
    {
        if(m_parse_option.include_layer(m_modal_variables.m_layer, m_modal_variables.m_datatype))
        {
            OasisRepetitionBuilder builder(m_parse_option.do_partition());
            std::vector<OasisRepetition> repetitions;
            std::vector<Point> offsets;
            if(builder.partition(m_modal_variables.m_repetition, m_layout->repetition_dimension_limit(), repetitions, offsets))
            {
                for(uint32 i = 0; i < repetitions.size(); ++i)
                {
                    PolygonElement* e = new PolygonElement;
                    OasisRepetition& repetition = repetitions[i];
                    Point& offset = offsets[i];
                    e->create(
                        m_modal_variables.m_geometry_x + offset.x(),
                        m_modal_variables.m_geometry_y + offset.y(),
                        m_modal_variables.m_polygon_pointlist,
                        ld.layer,
                        ld.datatype,
                        (repetition.valid() ? &repetition : NULL)
                        );
                    cell.add_geometry(e);
                }
            }
            else
            {
                PolygonElement* e = new PolygonElement;
                e->create(
                    m_modal_variables.m_geometry_x,
                    m_modal_variables.m_geometry_y,
                    m_modal_variables.m_polygon_pointlist,
                    ld.layer,
                    ld.datatype,
                    &m_modal_variables.m_repetition
                    );
                cell.add_geometry(e);
            }
        }
    }
    else
    {
        if(m_parse_option.include_layer(m_modal_variables.m_layer, m_modal_variables.m_datatype))
        {
            PolygonElement* e = new PolygonElement;
            e->create(
                    m_modal_variables.m_geometry_x,
                    m_modal_variables.m_geometry_y,
                    m_modal_variables.m_polygon_pointlist,
                    ld.layer,
                    ld.datatype
                    );
            cell.add_geometry(e);
        }
    }
}

void OasisParser::read_path(const OasisRecordId& rid, OasisCell& cell)
{
    using namespace PathInfoByte;
    oas_assert(rid == ORID_Path);

    uint8 info_byte = m_reader.read_byte();

    if(info_byte & HasLayer)
    {
        m_modal_variables.m_layer = m_reader.read_uint();
        m_modal_variables.define_bit(ModalVariable::LayerBit);
    }

    if(info_byte & HasDatatype)
    {
        m_modal_variables.m_datatype = m_reader.read_uint();
        m_modal_variables.define_bit(ModalVariable::DataTypeBit);
    }

    if(info_byte & HasHalfWidth)
    {
        m_modal_variables.m_path_halfwidth = m_reader.read_uint();
        m_modal_variables.define_bit(ModalVariable::PathHalfWidthBit);
    }

    if(info_byte & HasExtension)
    {
        uint8 extension_scheme = m_reader.read_uint();
        uint8 start_scheme = (extension_scheme & 0x0c) >> 2;
        uint8 end_scheme = (extension_scheme & 0x03);

        if(start_scheme == FlushExtension)
        {
            m_modal_variables.m_path_start_extention = 0;
        }
        else if(start_scheme == HalfwidthExtension)
        {
            m_modal_variables.m_path_start_extention = m_modal_variables.m_path_halfwidth;
        }
        else if(start_scheme == ExplicitExtension)
        {
            m_modal_variables.m_path_start_extention = m_reader.read_int();
        }
        else
        {
            oas_assert(m_modal_variables.defined_bit(ModalVariable::PathStartExtBit));
        }

        if(end_scheme == FlushExtension)
        {
            m_modal_variables.m_path_end_extention = 0;
        }
        else if(end_scheme == HalfwidthExtension)
        {
            m_modal_variables.m_path_end_extention = m_modal_variables.m_path_halfwidth;
        }
        else if(end_scheme == ExplicitExtension)
        {
            m_modal_variables.m_path_end_extention = m_reader.read_int();
        }
        else
        {
            oas_assert(m_modal_variables.defined_bit(ModalVariable::PathEndExtBit));
        }

        m_modal_variables.define_bit(ModalVariable::PathStartExtBit);
        m_modal_variables.define_bit(ModalVariable::PathEndExtBit);
    }

    if(info_byte & HasPointList)
    {
        m_reader.read_point_list(m_modal_variables.m_path_pointlist);
        m_modal_variables.define_bit(ModalVariable::PathPointListBit);
    }

    if(info_byte & HasX)
    {
        if(m_modal_variables.m_xymode == ModalVariable::XYAbsolute)
        {
            m_modal_variables.m_geometry_x = m_reader.read_int();
        }
        else
        {
            m_modal_variables.m_geometry_x = m_reader.read_int() + m_modal_variables.m_geometry_x;
        }
        m_modal_variables.define_bit(ModalVariable::GeometryXBit);
    }

    if(info_byte & HasY)
    {
        if(m_modal_variables.m_xymode == ModalVariable::XYAbsolute)
        {
            m_modal_variables.m_geometry_y = m_reader.read_int();
        }
        else
        {
            m_modal_variables.m_geometry_y = m_reader.read_int() + m_modal_variables.m_geometry_y;
        }
        m_modal_variables.define_bit(ModalVariable::GeometryYBit);
    }

    if (info_byte & HasRepetition)
    {
        m_reader.read_repetition(m_modal_variables.m_repetition);
        m_modal_variables.define_bit(ModalVariable::RepetitionBit);
    }

    read_element_properties();

    if (m_parse_option.parse_over())
    {
        return;
    }

    oas_assert(m_modal_variables.defined_bit(ModalVariable::GeometryXBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::GeometryYBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::PathStartExtBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::PathEndExtBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::PathHalfWidthBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::PathPointListBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::LayerBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::DataTypeBit));

    LDType ld = m_parse_option.map_layer(m_modal_variables.m_layer, m_modal_variables.m_datatype);
    if(info_byte & HasRepetition)
    {
        if(m_parse_option.include_layer(m_modal_variables.m_layer, m_modal_variables.m_datatype))
        {
            OasisRepetitionBuilder builder(m_parse_option.do_partition());
            std::vector<OasisRepetition> repetitions;
            std::vector<Point> offsets;
            if(builder.partition(m_modal_variables.m_repetition, m_layout->repetition_dimension_limit(), repetitions, offsets))
            {
                for(uint32 i = 0; i < repetitions.size(); ++i)
                {
                    PathElement* e = new PathElement;
                    OasisRepetition& repetition = repetitions[i];
                    Point& offset = offsets[i];
                    e->create(
                        m_modal_variables.m_geometry_x + offset.x(),
                        m_modal_variables.m_geometry_y + offset.y(),
                        m_modal_variables.m_path_start_extention,
                        m_modal_variables.m_path_end_extention,
                        m_modal_variables.m_path_halfwidth,
                        m_modal_variables.m_path_pointlist,
                        ld.layer,
                        ld.datatype,
                        (repetition.valid() ? &repetition : NULL)
                        );
                    cell.add_geometry(e);
                }
            }
            else
            {
                PathElement* e = new PathElement;
                e->create(
                    m_modal_variables.m_geometry_x,
                    m_modal_variables.m_geometry_y,
                    m_modal_variables.m_path_start_extention,
                    m_modal_variables.m_path_end_extention,
                    m_modal_variables.m_path_halfwidth,
                    m_modal_variables.m_path_pointlist,
                    ld.layer,
                    ld.datatype,
                    &m_modal_variables.m_repetition
                    );
                cell.add_geometry(e);
            }
        }
    }
    else
    {
        if(m_parse_option.include_layer(m_modal_variables.m_layer, m_modal_variables.m_datatype))
        {
            PathElement* e = new PathElement;
            e->create(
                m_modal_variables.m_geometry_x,
                m_modal_variables.m_geometry_y,
                m_modal_variables.m_path_start_extention,
                m_modal_variables.m_path_end_extention,
                m_modal_variables.m_path_halfwidth,
                m_modal_variables.m_path_pointlist,
                ld.layer,
                ld.datatype
                );
            cell.add_geometry(e);
        }
    }
}

void OasisParser::read_trapezoid(const OasisRecordId& rid, OasisCell& cell)
{
    using namespace TrapezoidInfoByte;
    oas_assert(rid == ORID_Trapezoid || rid == ORID_TrapezoidA || rid == ORID_TrapezoidB);

    uint8 info_byte = m_reader.read_byte();

    if(info_byte & HasLayer)
    {
        m_modal_variables.m_layer = m_reader.read_uint();
        m_modal_variables.define_bit(ModalVariable::LayerBit);
    }

    if(info_byte & HasDatatype)
    {
        m_modal_variables.m_datatype = m_reader.read_uint();
        m_modal_variables.define_bit(ModalVariable::DataTypeBit);
    }

    if(info_byte & HasWidth)
    {
        m_modal_variables.m_geometry_w = m_reader.read_uint();
        m_modal_variables.define_bit(ModalVariable::GeometryWBit);
    }

    if(info_byte & HasHeight)
    {
        m_modal_variables.m_geometry_h = m_reader.read_uint();
        m_modal_variables.define_bit(ModalVariable::GeometryHBit);
    }

    int64 delta_a = 0;
    int64 delta_b = 0;
    if(rid == ORID_Trapezoid || rid == ORID_TrapezoidA)
    {
        delta_a = m_reader.read_int();
    }
    if(rid == ORID_Trapezoid || rid == ORID_TrapezoidB)
    {
        delta_b = m_reader.read_int();
    }

    if(info_byte & HasX)
    {
        if(m_modal_variables.m_xymode == ModalVariable::XYAbsolute)
        {
            m_modal_variables.m_geometry_x = m_reader.read_int();
        }
        else
        {
            m_modal_variables.m_geometry_x = m_reader.read_int() + m_modal_variables.m_geometry_x;
        }
        m_modal_variables.define_bit(ModalVariable::GeometryXBit);
    }

    if(info_byte & HasY)
    {
        if(m_modal_variables.m_xymode == ModalVariable::XYAbsolute)
        {
            m_modal_variables.m_geometry_y = m_reader.read_int();
        }
        else
        {
            m_modal_variables.m_geometry_y = m_reader.read_int() + m_modal_variables.m_geometry_y;
        }
        m_modal_variables.define_bit(ModalVariable::GeometryYBit);
    }

    if (info_byte & HasRepetition)
    {
        m_reader.read_repetition(m_modal_variables.m_repetition);
        m_modal_variables.define_bit(ModalVariable::RepetitionBit);
    }

    read_element_properties();

    if (m_parse_option.parse_over())
    {
        return;
    }

    oas_assert(m_modal_variables.defined_bit(ModalVariable::GeometryWBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::GeometryHBit));

    bool is_vertical = (info_byte & IsVertical) != 0;

    std::vector<Point> pts(4);
    if(is_vertical)
    {
        pts[0].set_x(0);
        pts[1].set_x(0);
        pts[2].set_x(m_modal_variables.m_geometry_w);
        pts[3].set_x(m_modal_variables.m_geometry_w);
        if(delta_a < 0)
        {
            pts[0].set_y(0);
            pts[3].set_y(-delta_a);
        }
        else
        {
            pts[0].set_y(delta_a);
            pts[3].set_y(0);
        }
        if(delta_b < 0)
        {
            pts[1].set_y(m_modal_variables.m_geometry_h + delta_b);
            pts[2].set_y(m_modal_variables.m_geometry_h);
        }
        else
        {
            pts[1].set_y(m_modal_variables.m_geometry_h);
            pts[2].set_y(m_modal_variables.m_geometry_h - delta_b);
        }

        if(pts[0].y() > pts[1].y() || pts[2].y() < pts[3].y())
        {
            throw OasisException("bad vertical trapezoid");
        }
        else if(pts[0].y() == pts[1].y())
        {
            pts[1] = pts[2];
            pts[2] = pts[3];
            pts.pop_back();
        }
        else if(pts[2].y() == pts[3].y())
        {
            pts.pop_back();
        }
    }
    else
    {
        pts[0].set_y(0);
        pts[1].set_y(m_modal_variables.m_geometry_h);
        pts[2].set_y(m_modal_variables.m_geometry_h);
        pts[3].set_y(0);
        if(delta_a < 0)
        {
            pts[0].set_x(-delta_a);
            pts[1].set_x(0);
        }
        else
        {
            pts[0].set_x(0);
            pts[1].set_x(delta_a);
        }
        if(delta_b < 0)
        {
            pts[2].set_x(m_modal_variables.m_geometry_w + delta_b);
            pts[3].set_x(m_modal_variables.m_geometry_w);
        }
        else
        {
            pts[2].set_x(m_modal_variables.m_geometry_w);
            pts[3].set_x(m_modal_variables.m_geometry_w - delta_b);
        }

        if(pts[0].x() > pts[3].x() || pts[1].x() > pts[2].x())
        {
            throw OasisException("bad horizontal trapezoid");
        }
        else if(pts[0].x() == pts[3].x())
        {
            pts.pop_back();
        }
        else if(pts[1].x() == pts[2].x())
        {
            pts[2] = pts[3];
            pts.pop_back();
        }
    }

    oas_assert(m_modal_variables.defined_bit(ModalVariable::GeometryXBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::GeometryYBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::LayerBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::DataTypeBit));

    LDType ld = m_parse_option.map_layer(m_modal_variables.m_layer, m_modal_variables.m_datatype);
    if(info_byte & HasRepetition)
    {
        if(m_parse_option.include_layer(m_modal_variables.m_layer, m_modal_variables.m_datatype))
        {
            OasisRepetitionBuilder builder(m_parse_option.do_partition());
            std::vector<OasisRepetition> repetitions;
            std::vector<Point> offsets;
            if(builder.partition(m_modal_variables.m_repetition, m_layout->repetition_dimension_limit(), repetitions, offsets))
            {
                for(uint32 i = 0; i < repetitions.size(); ++i)
                {
                    PolygonElement* e = new PolygonElement;
                    OasisRepetition& repetition = repetitions[i];
                    Point& offset = offsets[i];
                    e->create(
                        m_modal_variables.m_geometry_x + pts[0].y() + offset.x(),
                        m_modal_variables.m_geometry_y + pts[0].y() + offset.y(),
                        pts,
                        ld.layer,
                        ld.datatype,
                        (repetition.valid() ? &repetition : NULL)
                        );
                    cell.add_geometry(e);
                }
            }
            else
            {
                PolygonElement* e = new PolygonElement;
                e->create(
                    m_modal_variables.m_geometry_x + pts[0].x(),
                    m_modal_variables.m_geometry_y + pts[0].y(),
                    pts,
                    ld.layer,
                    ld.datatype,
                    &m_modal_variables.m_repetition
                    );
                cell.add_geometry(e);
            }
        }
    }
    else
    {
        if(m_parse_option.include_layer(m_modal_variables.m_layer, m_modal_variables.m_datatype))
        {
            PolygonElement* e = new PolygonElement;
            e->create(
                m_modal_variables.m_geometry_x + pts[0].x(),
                m_modal_variables.m_geometry_y + pts[0].y(),
                pts,
                ld.layer,
                ld.datatype
                );
            cell.add_geometry(e);
        }
    }
}

void OasisParser::read_c_trapezoid(const OasisRecordId& rid, OasisCell& cell)
{
    using namespace CTrapezoidInfoByte;
    oas_assert(rid == ORID_CTrapezoid);

    uint8 info_byte = m_reader.read_byte();

    if(info_byte & HasLayer)
    {
        m_modal_variables.m_layer = m_reader.read_uint();
        m_modal_variables.define_bit(ModalVariable::LayerBit);
    }

    if(info_byte & HasDatatype)
    {
        m_modal_variables.m_datatype = m_reader.read_uint();
        m_modal_variables.define_bit(ModalVariable::DataTypeBit);
    }

    if(info_byte & HasTrapType)
    {
        m_modal_variables.m_ctrapezoid_type = m_reader.read_uint();
        m_modal_variables.define_bit(ModalVariable::CTrapezoidTypeBit);
    }

    if(info_byte & HasWidth)
    {
        m_modal_variables.m_geometry_w = m_reader.read_uint();
        m_modal_variables.define_bit(ModalVariable::GeometryWBit);
    }

    if(info_byte & HasHeight)
    {
        m_modal_variables.m_geometry_h = m_reader.read_uint();
        m_modal_variables.define_bit(ModalVariable::GeometryHBit);
    }

    if(info_byte & HasX)
    {
        if(m_modal_variables.m_xymode == ModalVariable::XYAbsolute)
        {
            m_modal_variables.m_geometry_x = m_reader.read_int();
        }
        else
        {
            m_modal_variables.m_geometry_x = m_reader.read_int() + m_modal_variables.m_geometry_x;
        }
        m_modal_variables.define_bit(ModalVariable::GeometryXBit);
    }

    if(info_byte & HasY)
    {
        if(m_modal_variables.m_xymode == ModalVariable::XYAbsolute)
        {
            m_modal_variables.m_geometry_y = m_reader.read_int();
        }
        else
        {
            m_modal_variables.m_geometry_y = m_reader.read_int() + m_modal_variables.m_geometry_y;
        }
        m_modal_variables.define_bit(ModalVariable::GeometryYBit);
    }

    static int64 ctraps_table[][4][4] =
    {
        { {0, 0, 0, 0}, {0, 0, 0, 1}, {1, -1, 0, 1}, {1, 0, 0, 0} },
        { {0, 0, 0, 0}, {0, 0, 0, 1}, {1, 0, 0, 1}, {1, -1, 0, 0} },
        { {0, 0, 0, 0}, {0, 1, 0, 1}, {1, 0, 0, 1}, {1, 0, 0, 0} },
        { {0, 1, 0, 0}, {0, 0, 0, 1}, {1, 0, 0, 1}, {1, 0, 0, 0} },
        { {0, 0, 0, 0}, {0, 1, 0, 1}, {1, -1, 0, 1}, {1, 0, 0, 0} },
        { {0, 1, 0, 0}, {0, 0, 0, 1}, {1, 0, 0, 1}, {1, -1, 0, 0} },
        { {0, 0, 0, 0}, {0, 1, 0, 1}, {1, 0, 0, 1}, {1, -1, 0, 0} },
        { {0, 1, 0, 0}, {0, 0, 0, 1}, {1, -1, 0, 1}, {1, 0, 0, 0} },
        { {0, 0, 0, 0}, {0, 0, 0, 1}, {1, 0, -1, 1}, {1, 0, 0, 0} },
        { {0, 0, 0, 0}, {0, 0, -1, 1}, {1, 0, 0, 1}, {1, 0, 0, 0} },
        { {0, 0, 0, 0}, {0, 0, 0, 1}, {1, 0, 0, 1}, {1, 0, 1, 0} },
        { {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 1}, {1, 0, 0, 0} },
        { {0, 0, 0, 0}, {0, 0, 0, 1}, {1, 0, -1, 1}, {1, 0, 1, 0} },
        { {0, 0, 1, 0}, {0, 0, -1, 1}, {1, 0, 0, 1}, {1, 0, 0, 0} },
        { {0, 0, 0, 0}, {0, 0, -1, 1}, {1, 0, 0, 1}, {1, 0, 1, 0} },
        { {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, -1, 1}, {1, 0, 0, 0} },
        { {0, 0, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0} },
        { {0, 0, 0, 0}, {0, 0, 1, 0}, {1, 0, 1, 0}, {0, 0, 0, 0} },
        { {0, 0, 0, 0}, {1, 0, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0} },
        { {0, 0, 1, 0}, {1, 0, 1, 0}, {1, 0, 0, 0}, {0, 0, 1, 0} },
        { {0, 0, 0, 0}, {0, 1, 0, 1}, {0, 2, 0, 0}, {0, 0, 0, 0} },
        { {0, 0, 0, 1}, {0, 2, 0, 1}, {0, 1, 0, 0}, {0, 0, 0, 1} },
        { {0, 0, 0, 0}, {0, 0, 2, 0}, {1, 0, 1, 0}, {0, 0, 0, 0} },
        { {1, 0, 0, 0}, {0, 0, 1, 0}, {1, 0, 2, 0}, {1, 0, 0, 0} },
        { {0, 0, 0, 0}, {0, 0, 0, 1}, {1, 0, 0, 1}, {1, 0, 0, 0} },
        { {0, 0, 0, 0}, {0, 0, 1, 0}, {1, 0, 1, 0}, {1, 0, 0, 0} },
    };

    oas_assert(m_modal_variables.defined_bit(ModalVariable::CTrapezoidTypeBit));

    if (m_modal_variables.m_ctrapezoid_type <= 3)
    {
        oas_assert(m_modal_variables.m_geometry_w >= m_modal_variables.m_geometry_h);
    }
    else if (m_modal_variables.m_ctrapezoid_type <= 5)
    {
        oas_assert(m_modal_variables.m_geometry_w >= 2 * m_modal_variables.m_geometry_h);
    }
    else if (m_modal_variables.m_ctrapezoid_type <= 7)
    {//document error?
        oas_assert(m_modal_variables.m_geometry_w >= m_modal_variables.m_geometry_h);
    }
    else if (m_modal_variables.m_ctrapezoid_type <= 11)
    {
        oas_assert(m_modal_variables.m_geometry_h >= m_modal_variables.m_geometry_w);
    }
    else if (m_modal_variables.m_ctrapezoid_type <= 13)
    {
        oas_assert(m_modal_variables.m_geometry_h >= 2 * m_modal_variables.m_geometry_w);
    }
    else if (m_modal_variables.m_ctrapezoid_type <= 15)
    {//document error?
        oas_assert(m_modal_variables.m_geometry_h >= m_modal_variables.m_geometry_w);
    }
    else if (m_modal_variables.m_ctrapezoid_type <= 19)
    {
        oas_assert((info_byte & HasHeight) == 0);
        m_modal_variables.m_geometry_h = m_modal_variables.m_geometry_w;
        m_modal_variables.define_bit(ModalVariable::GeometryHBit);
    }
    else if (m_modal_variables.m_ctrapezoid_type <= 21)
    {
        oas_assert((info_byte & HasWidth) == 0);
        m_modal_variables.m_geometry_w = m_modal_variables.m_geometry_h;
        m_modal_variables.define_bit(ModalVariable::GeometryWBit);
    }
    else if (m_modal_variables.m_ctrapezoid_type <= 23)
    {
        oas_assert((info_byte & HasHeight) == 0);
        m_modal_variables.m_geometry_h = m_modal_variables.m_geometry_w;
        m_modal_variables.define_bit(ModalVariable::GeometryHBit);
    }
    else if (m_modal_variables.m_ctrapezoid_type <= 24)
    {
    }
    else if (m_modal_variables.m_ctrapezoid_type <= 25)
    {
        oas_assert((info_byte & HasHeight) == 0);
        m_modal_variables.m_geometry_h = m_modal_variables.m_geometry_w;
        m_modal_variables.define_bit(ModalVariable::GeometryHBit);
    }

    oas_assert(m_modal_variables.defined_bit(ModalVariable::GeometryWBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::GeometryHBit));

    if (m_modal_variables.m_ctrapezoid_type >= sizeof(ctraps_table) / sizeof(ctraps_table[0]))
    {
        OAS_ERR << "Invalid CTRAPEZOID type " << m_modal_variables.m_ctrapezoid_type << OAS_ENDL;
        throw OasisException("invalid ctrapezoid type");
    }

    int64 w = 0;
    int64 h = 0;
    std::vector<Point> pts(4);
    for (int32 i = 0; i < 4; ++i)
    {
        int64 *m = ctraps_table[m_modal_variables.m_ctrapezoid_type][i];

        int64 x = 0;
        if (m[0] != 0) x += m[0] * m_modal_variables.m_geometry_w;
        if (m[1] != 0) x += m[1] * m_modal_variables.m_geometry_h;

        int64 y = 0;
        if (m[2] != 0) y += m[2] * m_modal_variables.m_geometry_w;
        if (m[3] != 0) y += m[3] * m_modal_variables.m_geometry_h;

        pts[i] = Point(x, y);

        if (x > w) w = x;
        if (y > h) h = y;
    }

    if(pts.back() == pts.front())
    {
        pts.pop_back();
    }

    m_modal_variables.m_geometry_w = w;
    m_modal_variables.m_geometry_h = h;

    if (info_byte & HasRepetition)
    {
        m_reader.read_repetition(m_modal_variables.m_repetition);
        m_modal_variables.define_bit(ModalVariable::RepetitionBit);
    }

    read_element_properties();

    if (m_parse_option.parse_over())
    {
        return;
    }

    oas_assert(m_modal_variables.defined_bit(ModalVariable::GeometryXBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::GeometryYBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::LayerBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::DataTypeBit));

    LDType ld = m_parse_option.map_layer(m_modal_variables.m_layer, m_modal_variables.m_datatype);
    if(info_byte & HasRepetition)
    {
        if(m_parse_option.include_layer(m_modal_variables.m_layer, m_modal_variables.m_datatype))
        {
            OasisRepetitionBuilder CTrap_builder(m_parse_option.do_partition());
            std::vector<OasisRepetition> CTrap_repetitions;
            std::vector<Point> CTrap_offsets;
            if(CTrap_builder.partition(m_modal_variables.m_repetition, m_layout->repetition_dimension_limit(), CTrap_repetitions, CTrap_offsets))
            {
                for(uint32 i = 0; i < CTrap_repetitions.size(); ++i)
                {
                    PolygonElement* e = new PolygonElement;
                    OasisRepetition& repetition = CTrap_repetitions[i];
                    Point& CTrap_offset = CTrap_offsets[i];
                    e->create(
                        m_modal_variables.m_geometry_x + pts[0].x() + CTrap_offset.x(),
                        m_modal_variables.m_geometry_y + pts[0].y() + CTrap_offset.y(),
                        pts,
                        ld.layer,
                        ld.datatype,
                        (repetition.valid() ? &repetition : NULL)
                        );
                    cell.add_geometry(e);
                }
            }
            else
            {
                PolygonElement* poly_element = new PolygonElement;
                poly_element->create(
                    m_modal_variables.m_geometry_x + pts[0].x(),
                    m_modal_variables.m_geometry_y + pts[0].y(),
                    pts,
                    ld.layer,
                    ld.datatype,
                    &m_modal_variables.m_repetition
                    );
                cell.add_geometry(poly_element);
            }
        }
    }
    else
    {
        if(m_parse_option.include_layer(m_modal_variables.m_layer, m_modal_variables.m_datatype))
        {
            PolygonElement* poly_element = new PolygonElement;
            poly_element->create(
                m_modal_variables.m_geometry_x + pts[0].x(),
                m_modal_variables.m_geometry_y + pts[0].y(),
                pts,
                ld.layer,
                ld.datatype
                );
            cell.add_geometry(poly_element);
        }
    }

}

void OasisParser::read_circle(const OasisRecordId& rid, OasisCell& cell)
{
    using namespace CircleInfoByte;
    oas_assert(rid == ORID_Circle);
    uint8 info_byte = m_reader.read_byte();

    if(info_byte & HasLayer)
    {
        m_modal_variables.m_layer = m_reader.read_uint();
        m_modal_variables.define_bit(ModalVariable::LayerBit);
    }

    if(info_byte & HasDatatype)
    {
        m_modal_variables.m_datatype = m_reader.read_uint();
        m_modal_variables.define_bit(ModalVariable::DataTypeBit);
    }

    if(info_byte & HasRadius)
    {
        m_modal_variables.m_circle_radius = m_reader.read_uint();
        m_modal_variables.define_bit(ModalVariable::CircleRadiusBit);
    }

    if(info_byte & HasX)
    {
        if(m_modal_variables.m_xymode == ModalVariable::XYAbsolute)
        {
            m_modal_variables.m_geometry_x = m_reader.read_int();
        }
        else
        {
            m_modal_variables.m_geometry_x = m_reader.read_int() + m_modal_variables.m_geometry_x;
        }
        m_modal_variables.define_bit(ModalVariable::GeometryXBit);
    }

    if(info_byte & HasY)
    {
        if(m_modal_variables.m_xymode == ModalVariable::XYAbsolute)
        {
            m_modal_variables.m_geometry_y = m_reader.read_int();
        }
        else
        {
            m_modal_variables.m_geometry_y = m_reader.read_int() + m_modal_variables.m_geometry_y;
        }
        m_modal_variables.define_bit(ModalVariable::GeometryYBit);
    }

    if (info_byte & HasRepetition)
    {
        m_reader.read_repetition(m_modal_variables.m_repetition);
        m_modal_variables.define_bit(ModalVariable::RepetitionBit);
    }

    read_element_properties();

    if (m_parse_option.parse_over())
    {
        return;
    }

    oas_assert(m_modal_variables.defined_bit(ModalVariable::CircleRadiusBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::GeometryXBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::GeometryYBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::LayerBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::DataTypeBit));

#define COS_225 0.923879533
#define SIN_225 0.382683432
    std::vector<Point> pts(8);
    int64 rcos = (int64)ceil(COS_225 * m_modal_variables.m_circle_radius);
    int64 rsin = (int64)ceil(SIN_225 * m_modal_variables.m_circle_radius);
    pts[0].set_x(-rcos); pts[0].set_y(-rsin);
    pts[1].set_x(-rcos); pts[1].set_y(rsin);
    pts[2].set_x(-rsin); pts[2].set_y(rcos);
    pts[3].set_x(rsin);  pts[3].set_y(rcos);
    pts[4].set_x(rcos);  pts[4].set_y(rsin);
    pts[5].set_x(rcos);  pts[5].set_y(-rsin);
    pts[6].set_x(rsin);  pts[6].set_y(-rcos);
    pts[7].set_x(-rsin); pts[7].set_y(-rcos);

    LDType ld = m_parse_option.map_layer(m_modal_variables.m_layer, m_modal_variables.m_datatype);
    if(info_byte & HasRepetition)
    {
        if(m_parse_option.include_layer(m_modal_variables.m_layer, m_modal_variables.m_datatype))
        {
            OasisRepetitionBuilder builder(m_parse_option.do_partition());
            std::vector<OasisRepetition> repetitions;
            std::vector<Point> offsets;
            if(builder.partition(m_modal_variables.m_repetition, m_layout->repetition_dimension_limit(), repetitions, offsets))
            {
                for(uint32 i = 0; i < repetitions.size(); ++i)
                {
                    PolygonElement* e = new PolygonElement;
                    OasisRepetition& repetition = repetitions[i];
                    Point offset = offsets[i];
                    e->create(
                        m_modal_variables.m_geometry_x + pts[0].x() + offset.x(),
                        m_modal_variables.m_geometry_y + pts[0].y() + offset.y(),
                        pts,
                        ld.layer,
                        ld.datatype,
                        (repetition.valid() ? &repetition : NULL)
                        );
                    cell.add_geometry(e);
                }
            }
            else
            {
                PolygonElement* e = new PolygonElement;
                e->create(
                    m_modal_variables.m_geometry_x + pts[0].x(),
                    m_modal_variables.m_geometry_y + pts[0].y(),
                    pts,
                    ld.layer,
                    ld.datatype,
                    &m_modal_variables.m_repetition
                    );
                cell.add_geometry(e);
            }
        }
    }
    else
    {
        if(m_parse_option.include_layer(m_modal_variables.m_layer, m_modal_variables.m_datatype))
        {
            PolygonElement* e = new PolygonElement;
            e->create(
                m_modal_variables.m_geometry_x + pts[0].x(),
                m_modal_variables.m_geometry_y + pts[0].y(),
                pts,
                ld.layer,
                ld.datatype
                );
            cell.add_geometry(e);
        }
    }
}

void OasisParser::read_element_properties()
{
    while(true)
    {
        OasisRecordId rid = read_record_id();
        OAS_DBG << "Record " << oasis_record_id_name(rid) << " in read element property" << OAS_ENDL;
        if(rid == ORID_Pad)
        {
            continue;
        }
        else if(rid == ORID_Cblock)
        {
            enter_cblock(rid);
            continue;
        }
        else if(rid == ORID_Property)
        {
            read_properties(rid);
        }
        else if(rid == ORID_PropertyLast)
        {
            if(!m_modal_variables.defined_bit(ModalVariable::LastPropertyBit))
            {
                throw OasisException("last property undefined");
            }
        }
        else
        {
            cache_id();
            break;
        }

        oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropertyBit));
        oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropStandardBit));
        oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropnameBit));
        oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropvaluesBit));
        OAS_DBG << "skip property" << OAS_ENDL;
    }
}

void OasisParser::read_cell_properties()
{
    while(true)
    {
        OasisRecordId rid = read_record_id();
        OAS_DBG << "Record " << oasis_record_id_name(rid) << " in read cell property" << OAS_ENDL;
        if(rid == ORID_Pad)
        {
            continue;
        }
        else if(rid == ORID_Cblock)
        {
            enter_cblock(rid);
            continue;
        }
        else if(rid == ORID_Property)
        {
            read_properties(rid);
        }
        else if(rid == ORID_PropertyLast)
        {
            if(!m_modal_variables.defined_bit(ModalVariable::LastPropertyBit))
            {
                throw OasisException("last property undefined");
            }
        }
        else
        {
            cache_id();
            break;
        }

        oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropertyBit));
        oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropStandardBit));
        oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropnameBit));
        oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropvaluesBit));

        OAS_DBG << "skip property" << OAS_ENDL;
    }
}

void OasisParser::read_cell_name_properties(OasisCellInfo& cellinfo)
{
    while(true)
    {
        OasisRecordId rid = read_record_id();
        OAS_DBG << "Record " << oasis_record_id_name(rid) << " in read cellname property" << OAS_ENDL;
        if(rid == ORID_Pad)
        {
            continue;
        }
        else if(rid == ORID_Cblock)
        {
            enter_cblock(rid);
            continue;
        }
        else if(rid == ORID_Property)
        {
            read_properties(rid);
        }
        else if(rid == ORID_PropertyLast)
        {
            if(!m_modal_variables.defined_bit(ModalVariable::LastPropertyBit))
            {
                throw OasisException("last property undefined");
            }
        }
        else
        {
            cache_id();
            break;
        }

        oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropertyBit));
        oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropStandardBit));
        oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropnameBit));
        oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropvaluesBit));

        if(m_parse_mode == IMPORT)
        {
            OasisProperty p;
            p.create(
                m_modal_variables.m_last_property_is_standard,
                m_modal_variables.m_last_propname,
                m_modal_variables.m_last_propvalues
                );
            cellinfo.add_property(p);
        }
        else
        {
            OAS_DBG << "skip cellname property" << OAS_ENDL;
        }
    }
    if(m_parse_mode == IMPORT)
    {
        cellinfo.apply_properties(*m_layout);
    }
}

void OasisParser::skip_name_properties()
{
    while(true)
    {
        OasisRecordId rid = read_record_id();
        OAS_DBG << "Record " << oasis_record_id_name(rid) << " in skip name property" << OAS_ENDL;
        if(rid == ORID_Pad)
        {
            continue;
        }
        else if(rid == ORID_Cblock)
        {
            enter_cblock(rid);
            continue;
        }
        else if(rid == ORID_Property)
        {
            read_properties(rid);
        }
        else if(rid == ORID_PropertyLast)
        {
            if(!m_modal_variables.defined_bit(ModalVariable::LastPropertyBit))
            {
                throw OasisException("last property undefined");
            }
        }
        else
        {
            cache_id();
            break;
        }

        oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropertyBit));
        oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropStandardBit));
        oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropnameBit));
        oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropvaluesBit));

        OAS_DBG << "skip name property" << OAS_ENDL;
    }
}

void OasisParser::read_file_properties()
{
    while(true)
    {
        OasisRecordId rid = read_record_id();
        OAS_DBG << "Record " << oasis_record_id_name(rid) << " in read file property" << OAS_ENDL;
        if(rid == ORID_Pad)
        {
            continue;
        }
        else if(rid == ORID_Cblock)
        {
            enter_cblock(rid);
            continue;
        }
        else if(rid == ORID_Property)
        {
            read_properties(rid);
        }
        else if(rid == ORID_PropertyLast)
        {
            if(!m_modal_variables.defined_bit(ModalVariable::LastPropertyBit))
            {
                throw OasisException("last property undefined");
            }
        }
        else
        {
            cache_id();
            break;
        }

        oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropertyBit));
        oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropStandardBit));
        oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropnameBit));
        oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropvaluesBit));

        if(m_parse_mode == IMPORT)
        {
            OasisProperty p;
            p.create(
                m_modal_variables.m_last_property_is_standard,
                m_modal_variables.m_last_propname,
                m_modal_variables.m_last_propvalues
                );
            m_layout->add_property(p);
        }
        else
        {
            OAS_DBG << "skip file property" << OAS_ENDL;
        }
    }
}

void OasisParser::read_properties(const OasisRecordId& rid)
{
    using namespace PropertyInfoByte;
    oas_assert(rid == ORID_Property);

    uint8 info_byte = m_reader.read_byte();

    if(info_byte & HasExplicitName)
    {
        int64 index;
        if(info_byte & HasRefnum)
        {
            uint64 ref_num = m_reader.read_uint();
            if(m_layout->m_propnameref2index.find(ref_num) == m_layout->m_propnameref2index.end())
            {
                oas_assert(!m_layout->propname_strictmode());

                index = m_layout->m_propnames.size();
                OasisPropName opn;
                opn.set_index(index);
                opn.set_refnum(ref_num);
                m_layout->m_propnames.push_back(opn);
                m_layout->m_propnameref2index[ref_num] = index;
            }
            else
            {
                index = m_layout->m_propnameref2index[ref_num];
                oas_assert((uint64)index < m_layout->m_propnames.size());
            }
        }
        else
        {
            oas_assert(!m_layout->propname_strictmode());

            OasisString propname;
            m_reader.read_string(propname);
            if(m_layout->m_propname2index.find(propname.value()) == m_layout->m_propname2index.end())
            {
                index = m_layout->m_propnames.size();
                OasisPropName opn;
                opn.set_index(index);
                opn.set_name(propname);
                m_layout->m_propnames.push_back(opn);
                m_layout->m_propname2index[propname.value()] = index;
            }
            else
            {
                index = m_layout->m_propname2index[propname.value()];
                oas_assert((uint64)index < m_layout->m_propnames.size());
            }
        }
        m_modal_variables.m_last_propname = m_layout->m_propnames[index];
        m_modal_variables.define_bit(ModalVariable::LastPropnameBit);
    }
    else
    {
        oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropnameBit));
    }

    m_modal_variables.m_last_property_is_standard = ((info_byte & IsStandard) != 0);
    m_modal_variables.define_bit(ModalVariable::LastPropStandardBit);

    if(!(info_byte & ReuseLastValue))
    {
        uint64 value_count = ((info_byte & ValueCountMask) >> 4);
        if(value_count == 15)
        {
            value_count = m_reader.read_uint();
        }

        m_modal_variables.m_last_propvalues.resize(value_count);

        for(uint32 i = 0; i < value_count; ++i)
        {
            OasisPropValue& opv = m_modal_variables.m_last_propvalues[i];
            m_reader.read_prop_value(opv);
            if(opv.is_refstring())
            {
                uint64 ref_num = opv.get_refnum();
                if(m_layout->m_propstringref2index.find(ref_num) == m_layout->m_propstringref2index.end())
                {
                    oas_assert(!m_layout->propstring_strictmode());

                    int64 index = m_layout->m_propstrings.size();
                    OasisPropString ops;
                    ops.set_index(index);
                    ops.set_refnum(ref_num);
                    m_layout->m_propstrings.push_back(ops);
                    m_layout->m_propstringref2index[ref_num] = index;

                    opv.set_index(index);
                }
                else
                {
                    opv.set_index(m_layout->m_propstringref2index[ref_num]);
                    oas_assert((uint64)m_layout->m_propstringref2index[ref_num] < m_layout->m_propstrings.size());
                }
            }
            else if(opv.is_string())
            {
                oas_assert(!m_layout->propstring_strictmode());

                const OasisString& propstring = opv.get_string();
                if(m_layout->m_propstring2index.find(propstring.value()) == m_layout->m_propstring2index.end())
                {
                    int64 index = m_layout->m_propstrings.size();
                    OasisPropString ops;
                    ops.set_index(index);
                    ops.set_name(propstring);
                    m_layout->m_propstrings.push_back(ops);
                    m_layout->m_propstring2index[propstring.value()] = index;

                    opv.set_index(index);
                }
                else
                {
                    opv.set_index(m_layout->m_propstring2index[propstring.value()]);
                    oas_assert((uint64)m_layout->m_propstring2index[propstring.value()] < m_layout->m_propstrings.size());
                }
            }
        }

        m_modal_variables.define_bit(ModalVariable::LastPropvaluesBit);
    }
    else
    {
        oas_assert(!(info_byte & ValueCountMask));
        oas_assert(m_modal_variables.defined_bit(ModalVariable::LastPropvaluesBit));
    }

    m_modal_variables.define_bit(ModalVariable::LastPropertyBit);
}

void OasisParser::read_x_element(const OasisRecordId& rid, OasisCell& cell)
{
    oas_assert(rid == ORID_XElement);
    m_reader.read_uint();
    m_reader.skip_string();

    read_element_properties();

    if (m_parse_option.parse_over())
    {
        return;
    }
    OAS_DBG << "skip xelement" << OAS_ENDL;
}

void OasisParser::read_x_geometry(const OasisRecordId& rid, OasisCell& cell)
{
    using namespace XGeometryInfoByte;
    oas_assert(rid == ORID_XGeometry);
    uint8 info_byte = m_reader.read_byte();
    m_reader.read_uint();

    if(info_byte & HasLayer)
    {
        m_modal_variables.m_layer = m_reader.read_uint();
        m_modal_variables.define_bit(ModalVariable::LayerBit);
    }

    if(info_byte & HasDatatype)
    {
        m_modal_variables.m_datatype = m_reader.read_uint();
        m_modal_variables.define_bit(ModalVariable::DataTypeBit);
    }

    m_reader.skip_string();

    if(info_byte & HasX)
    {
        if(m_modal_variables.m_xymode == ModalVariable::XYAbsolute)
        {
            m_modal_variables.m_geometry_x = m_reader.read_int();
        }
        else
        {
            m_modal_variables.m_geometry_x = m_reader.read_int() + m_modal_variables.m_geometry_x;
        }
        m_modal_variables.define_bit(ModalVariable::GeometryXBit);
    }

    if(info_byte & HasY)
    {
        if(m_modal_variables.m_xymode == ModalVariable::XYAbsolute)
        {
            m_modal_variables.m_geometry_y = m_reader.read_int();
        }
        else
        {
            m_modal_variables.m_geometry_y = m_reader.read_int() + m_modal_variables.m_geometry_y;
        }
        m_modal_variables.define_bit(ModalVariable::GeometryYBit);
    }

    if(info_byte & HasRepetition)
    {
        m_reader.read_repetition(m_modal_variables.m_repetition);
        m_modal_variables.define_bit(ModalVariable::RepetitionBit);
    }

    read_element_properties();

    if (m_parse_option.parse_over())
    {
        return;
    }

    oas_assert(m_modal_variables.defined_bit(ModalVariable::LayerBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::DataTypeBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::GeometryXBit));
    oas_assert(m_modal_variables.defined_bit(ModalVariable::GeometryYBit));
    OAS_DBG << "skip xgeometry" << OAS_ENDL;
}

void OasisParser::enter_cblock(const OasisRecordId& rid)
{
    oas_assert(rid == ORID_Cblock);
    uint64 comp_type = m_reader.read_uint();
    oas_assert(comp_type == 0);
    (void)comp_type;
    uint64 uncomp_byte_count = m_reader.read_uint();
    uint64 comp_byte_count = m_reader.read_uint();
    m_reader.enter_cblock(uncomp_byte_count, comp_byte_count);
}

void OasisParser::parse_cell_name(const OasisRecordId& rid, bool skip)
{
    oas_assert(rid == ORID_CellnameRef || rid == ORID_Cellname);
    OAS_DBG << "Record " << oasis_record_id_name(rid) << OAS_ENDL;

    m_modal_variables.reset();

    OasisString cell_name;
    m_reader.read_string(cell_name);
    uint64 ref_num = rid == ORID_Cellname ? m_cellname_refnum : m_reader.read_uint();

    if (skip)
    {
        skip_name_properties();
        return;
    }

    ++m_cellname_refnum;

    if (m_parse_mode == PARSE && is_generated_cellname(cell_name.value()))
    {
        std::string new_value = rename_generated_prefix(cell_name.value());
        cell_name.set_value(new_value);
    }

    if(rid == ORID_Cellname)
    {
        if(m_cellname_idmode == EXPLICIT)
        {
            throw OasisException("cellname idmode error, previous implicit, current explicit");
        }
        m_cellname_idmode = IMPLICIT;
    }
    else
    {
        if(m_cellname_idmode == IMPLICIT)
        {
            throw OasisException("cellname idmode error, previous explicit, current implicit");
        }
        m_cellname_idmode = EXPLICIT;
    }

    std::map<uint64, int64>::iterator refit = m_layout->m_cellref2index.find(ref_num);
    std::map<std::string , int64>::iterator nameit = m_layout->m_cellname2index.find(cell_name.value());

    if(m_parse_mode == IMPORT)
    {
        oas_assert(refit == m_layout->m_cellref2index.end() && nameit == m_layout->m_cellname2index.end());
    }

    if(refit != m_layout->m_cellref2index.end() && nameit != m_layout->m_cellname2index.end())
    {
        oas_assert(m_parse_mode == PARSE);
        OasisCellInfo& rinfo = m_layout->get_cell_info(refit->second);
        OasisCellInfo& ninfo = m_layout->get_cell_info(nameit->second);

        ninfo.set_alias(refit->second);
        rinfo.set_alias(nameit->second);
        ninfo.set_ref_num(rinfo.get_ref_num());
        rinfo.set_name(ninfo.get_name());
        ninfo.set_flag(rinfo.get_flag());
        rinfo.set_flag(ninfo.get_flag());

        ninfo.set_named();
        rinfo.set_named();
    }
    else if(refit != m_layout->m_cellref2index.end())
    {
        m_layout->m_cellname2index[cell_name.value()] = refit->second;
        OasisCellInfo& info = m_layout->get_cell_info(refit->second);
        info.set_name(cell_name.value());
        info.set_named();
    }
    else if(nameit != m_layout->m_cellname2index.end())
    {
        m_layout->m_cellref2index[ref_num] = nameit->second;
        OasisCellInfo& info = m_layout->get_cell_info(nameit->second);
        info.set_ref_num(ref_num);
        info.set_named();
    }
    else
    {
        int64 index = m_layout->create_cell_info(ref_num, cell_name.value());
        OasisCellInfo& info = m_layout->get_cell_info(index);
        info.set_named();

        if(m_parse_mode == IMPORT)
        {
            int64 cellindex = m_layout->create_cell(true);
            oas_assert(cellindex == index && (int64)ref_num == index);
            OasisCell& cell = m_layout->get_cell(cellindex);
            cell.set_infoindex(index);
            info.set_cell_index(cellindex);
        }

        m_layout->m_cellref2index[ref_num] = index;
        m_layout->m_cellname2index[cell_name.value()] = index;
    }

    std::map<uint64, OasisString>::iterator it = m_layout->m_cellnames.find(ref_num);
    if(it != m_layout->m_cellnames.end())
    {
        throw OasisException("redefine cellname");
    }
    m_layout->m_cellnames[ref_num] = cell_name;

    if (m_layout->cellname_strictmode())
    {
        int64 index = m_layout->m_cellref2index[ref_num];
        read_cell_name_properties(m_layout->get_cell_info(index));
    }
    else
    {
        skip_name_properties();
    }
}

void OasisParser::parse_text_string(const OasisRecordId& rid, bool skip)
{
    oas_assert(rid == ORID_TextstringRef || rid == ORID_Textstring);

    m_modal_variables.reset();

    OasisString text_string;
    m_reader.read_string(text_string);
    uint64 ref_num = rid == ORID_Textstring ? m_textstring_refnum : m_reader.read_uint();

    if (skip)
    {
        skip_name_properties();
        return;
    }

    ++m_textstring_refnum;

    if(rid == ORID_Textstring)
    {
        if(m_textstring_idmode == EXPLICIT)
        {
            throw OasisException("textstring idmode error, previous implicit, current explicit");
        }
        m_textstring_idmode = IMPLICIT;
    }
    else
    {
        if(m_textstring_idmode == IMPLICIT)
        {
            throw OasisException("textstring idmode error, previous explicit, current implicit");
        }
        m_textstring_idmode = EXPLICIT;
    }

    std::map<uint64, int64>::iterator rit = m_layout->m_textref2index.find(ref_num);
    std::map<std::string, int64>::iterator sit = m_layout->m_textstring2index.find(text_string.value());

    if(m_parse_mode == IMPORT)
    {
        oas_assert(rit == m_layout->m_textref2index.end() && sit == m_layout->m_textstring2index.end());
    }

    if(rit != m_layout->m_textref2index.end() && sit != m_layout->m_textstring2index.end())
    {
        int64 rindex = rit->second;
        int64 sindex = sit->second;
        if(rindex == sindex)
        {
            throw OasisException("redefine textstring by same refnum");
        }
        else
        {
            OasisTextString& rots = m_layout->m_textstrings[rindex];
            OasisTextString& sots = m_layout->m_textstrings[sindex];
            rots.set_alias(sindex);
            sots.set_alias(rindex);
            rots.set_name(sots.get_name());
            sots.set_refnum(rots.get_refnum());
        }
    }
    else if(rit != m_layout->m_textref2index.end())
    {
        int64 index = rit->second;
        OasisTextString& ots = m_layout->m_textstrings[index];
        oas_assert(ots.is_referenced());
        ots.set_name(text_string);
        m_layout->m_textstring2index[text_string.value()] = index;
    }
    else if(sit != m_layout->m_textstring2index.end())
    {
        int64 index = sit->second;
        OasisTextString& ots = m_layout->m_textstrings[index];
        oas_assert(ots.is_named());
        ots.set_refnum(ref_num);
        m_layout->m_textref2index[ref_num] = index;
    }
    else
    {
        int64 index = m_layout->m_textstrings.size();
        if(m_parse_mode == IMPORT)
        {
            oas_assert(index == (int64)ref_num);
        }
        OasisTextString ots;
        ots.set_index(index);
        ots.set_refnum(ref_num);
        ots.set_name(text_string);
        m_layout->m_textstrings.push_back(ots);
        m_layout->m_textref2index[ref_num] = index;
        m_layout->m_textstring2index[text_string.value()] = index;
    }

    skip_name_properties();
}

void OasisParser::parse_layer_name(const OasisRecordId& rid, bool skip)
{
    oas_assert(rid == ORID_Layername || rid == ORID_LayernameText);

    m_modal_variables.reset();

    OasisString layer_name;
    m_reader.read_string(layer_name);
    OasisInterval layer_interval;
    m_reader.read_interval(layer_interval);
    OasisInterval datatype_interval;
    m_reader.read_interval(datatype_interval);
    skip_name_properties();

    if (skip)
    {
        return;
    }

    m_layout->m_layernames.push_back(OasisLayerName(layer_name, layer_interval, datatype_interval, rid == ORID_LayernameText));
}

void OasisParser::parse_prop_name(const OasisRecordId& rid, bool skip)
{
    oas_assert(rid == ORID_PropnameRef || rid == ORID_Propname);

    m_modal_variables.reset();

    OasisString prop_name;
    m_reader.read_string(prop_name);
    uint64 ref_num = rid == ORID_Propname ? m_propname_refnum : m_reader.read_uint();

    if (skip)
    {
        skip_name_properties();
        return;
    }

    ++m_propname_refnum;

    if(rid == ORID_Propname)
    {
        if(m_propname_idmode == EXPLICIT)
        {
            throw OasisException("propname idmode error, previous implicit, current explicit");
        }
        m_propname_idmode = IMPLICIT;
    }
    else
    {
        if(m_propname_idmode == IMPLICIT)
        {
            throw OasisException("propname idmode error, previous explicit, current implicit");
        }
        m_propname_idmode = EXPLICIT;
    }

    std::map<uint64, int64>::iterator rit = m_layout->m_propnameref2index.find(ref_num);
    std::map<std::string, int64>::iterator sit = m_layout->m_propname2index.find(prop_name.value());

    if(m_parse_mode == IMPORT)
    {
        oas_assert(rit == m_layout->m_propnameref2index.end() && sit == m_layout->m_propname2index.end());
    }

    if(rit != m_layout->m_propnameref2index.end() && sit != m_layout->m_propname2index.end())
    {
        int64 rindex = rit->second;
        int64 sindex = sit->second;
        if(rindex == sindex)
        {
            throw OasisException("redefine propname by same refnum");
        }
        else
        {
            OasisPropName& ropn = m_layout->m_propnames[rindex];
            OasisPropName& sopn = m_layout->m_propnames[sindex];
            ropn.set_alias(sindex);
            sopn.set_alias(rindex);
            ropn.set_name(sopn.get_name());
            sopn.set_refnum(ropn.get_refnum());
        }
    }
    else if(rit != m_layout->m_propnameref2index.end())
    {
        int64 index = rit->second;
        OasisPropName& opn = m_layout->m_propnames[index];
        oas_assert(opn.is_referenced());
        opn.set_name(prop_name);
        m_layout->m_propname2index[prop_name.value()] = index;
    }
    else if(sit != m_layout->m_propname2index.end())
    {
        int64 index = sit->second;
        OasisPropName& opn = m_layout->m_propnames[index];
        oas_assert(opn.is_named());
        opn.set_refnum(ref_num);
        m_layout->m_propnameref2index[ref_num] = index;
    }
    else
    {
        int64 index = m_layout->m_propnames.size();
        if(m_parse_mode == IMPORT)
        {
            oas_assert(index == (int64)ref_num);
        }
        OasisPropName opn;
        opn.set_index(index);
        opn.set_refnum(ref_num);
        opn.set_name(prop_name);
        m_layout->m_propnames.push_back(opn);
        m_layout->m_propnameref2index[ref_num] = index;
        m_layout->m_propname2index[prop_name.value()] = index;
    }

    skip_name_properties();
}

void OasisParser::parse_prop_string(const OasisRecordId& rid, bool skip)
{
    oas_assert(rid == ORID_PropstringRef || rid == ORID_Propstring);

    m_modal_variables.reset();

    OasisString prop_string;
    m_reader.read_string(prop_string);
    uint64 ref_num = rid == ORID_Propstring ? m_propstring_refnum : m_reader.read_uint();

    if (skip)
    {
        skip_name_properties();
        return;
    }

    ++m_propstring_refnum;

    if(rid == ORID_Propstring)
    {
        if(m_propstring_idmode == EXPLICIT)
        {
            throw OasisException("propstring idmode error, previous implicit, current explicit");
        }
        m_propstring_idmode = IMPLICIT;
    }
    else
    {
        if(m_propstring_idmode == IMPLICIT)
        {
            throw OasisException("propstring idmode error, previous explicit, current implicit");
        }
        m_propstring_idmode = EXPLICIT;
    }

    std::map<uint64, int64>::iterator rit = m_layout->m_propstringref2index.find(ref_num);
    std::map<std::string, int64>::iterator sit = m_layout->m_propstring2index.find(prop_string.value());

    if(m_parse_mode == IMPORT)
    {
        oas_assert(rit == m_layout->m_propstringref2index.end() && sit == m_layout->m_propstring2index.end());
    }

    if(rit != m_layout->m_propstringref2index.end() && sit != m_layout->m_propstring2index.end())
    {
        int64 rindex = rit->second;
        int64 sindex = sit->second;
        if(rindex == sindex)
        {
            throw OasisException("redefine propname by same refnum");
        }
        else
        {
            OasisPropString& rops = m_layout->m_propstrings[rindex];
            OasisPropString& sops = m_layout->m_propstrings[sindex];
            rops.set_alias(sindex);
            sops.set_alias(rindex);
            rops.set_name(sops.get_name());
            sops.set_refnum(rops.get_refnum());
        }
    }
    else if(rit != m_layout->m_propstringref2index.end())
    {
        int64 index = rit->second;
        OasisPropString& ops = m_layout->m_propstrings[index];
        oas_assert(ops.is_referenced());
        ops.set_name(prop_string);
        m_layout->m_propstring2index[prop_string.value()] = index;
    }
    else if(sit != m_layout->m_propstring2index.end())
    {
        int64 index = sit->second;
        OasisPropString& ops = m_layout->m_propstrings[index];
        oas_assert(ops.is_named());
        ops.set_refnum(ref_num);
        m_layout->m_propstringref2index[ref_num] = index;
    }
    else
    {
        int64 index = m_layout->m_propstrings.size();
        if(m_parse_mode == IMPORT)
        {
            oas_assert(index == (int64)ref_num);
        }
        OasisPropString ops;
        ops.set_index(index);
        ops.set_refnum(ref_num);
        ops.set_name(prop_string);
        m_layout->m_propstrings.push_back(ops);
        m_layout->m_propstringref2index[ref_num] = index;
        m_layout->m_propstring2index[prop_string.value()] = index;
    }

    skip_name_properties();
}

void OasisParser::parse_x_name(const OasisRecordId& rid, bool skip)
{
    oas_assert(rid == ORID_XNameRef || rid == ORID_XName);

    m_modal_variables.reset();

    m_reader.read_uint();
    m_reader.skip_string();
    int64 refnum = rid == ORID_XName ? m_xname_refnum : m_reader.read_uint();

    if (skip)
    {
        skip_name_properties();
        return;
    }

    ++m_xname_refnum;

    if (rid == ORID_XName)
    {
        if(m_xname_idmode == EXPLICIT)
        {
            throw OasisException("xname idmode error, previous implicit, current explicit");
        }
        m_xname_idmode = IMPLICIT;
    }
    else
    {
        if(m_xname_idmode == IMPLICIT)
        {
            throw OasisException("xname idmode error, previous explicit, current implicit");
        }
        m_xname_idmode = EXPLICIT;
    }

    OAS_DBG << "skip xname" << OAS_ENDL;

    skip_name_properties();
    (void)refnum;
}

void OasisParser::load_cell(OasisLayout* layout, int64 infoindex)
{
    m_parse_mode = RELOAD;
    m_layout = layout;
    OasisRecordId rid = read_record_id();
    OAS_DBG << "Record " << oasis_record_id_name(rid) << OAS_ENDL;
    if(rid == ORID_Cell || rid == ORID_CellRef)
    {
        OasisCellInfo& cellinfo = layout->get_cell_info(infoindex);
        parse_cell(rid, cellinfo.get_cell_index());
    }
    else
    {
        oas_assert(false);
    }
}

bool OasisParser::check_file(OasisLayout* layout, uint32* sourcefile_sha1)
{
    bool error_disp = OasisDebug::oasis_error_disp();
    try 
    {
        OasisDebug::oasis_error_disp(false);
        
        oas_assert(layout != NULL);
        m_parse_mode = IMPORT;
        m_layout = layout;
        m_layout->cleanup();
        m_layout->open_file(false);
        m_reader.init(m_layout->load_fp());

        parse_magic();
        parse_start();
        
        m_layout->check_file_version();
        
        if (sourcefile_sha1 != NULL)
        {
           if (!m_layout->check_origin_file_sha1(sourcefile_sha1))
              return false;
        }

        m_reader.seek(-256, true);
        cache_id(false);
        parse_end();

        OasisDebug::oasis_error_disp(error_disp);
        return true;
    }
    catch(oasis::OasisException& e)
    {
        OasisDebug::oasis_error_disp(error_disp);
        return false;
    }
}

void OasisParser::import_file(OasisLayout* layout)
{
    OAS_INFO << "bengin import file" << OAS_ENDL;

    oas_assert(layout != NULL);
    m_parse_mode = IMPORT;
    m_layout = layout;
    m_layout->cleanup();
    m_layout->open_file(false);
    m_reader.init(m_layout->load_fp());

    parse_magic();
    parse_start();
    m_layout->check_file_version();

    m_layout->collect_top_cells();
    m_layout->compute_deep_information();

    OAS_INFO << "end import file" << OAS_ENDL;
}

void OasisParser::import_tables()
{
    m_cellname_idmode = UNKNOWN;
    m_textstring_idmode = UNKNOWN;
    m_propname_idmode = UNKNOWN;
    m_propstring_idmode = UNKNOWN;
    m_xname_idmode = UNKNOWN;
    m_cellname_refnum = 0;
    m_textstring_refnum = 0;
    m_propname_refnum = 0;
    m_propstring_refnum = 0;
    m_xname_refnum = 0;

    if (m_layout->textstring_strictmode())
    {
        if(m_layout->m_textstring_offset > 0)
        {
            parse_text_string_table();
        }
        else if(m_layout->m_textstring_offset == 0)
        {
            OAS_DBG << "textstring table absence" << OAS_ENDL;
        }
        else
        {
            throw OasisException("textstring table offset invalid");
        }
    }
    if (m_layout->propname_strictmode())
    {
        if(m_layout->m_propname_offset > 0)
        {
            parse_prop_name_table();
        }
        else if(m_layout->m_propname_offset == 0)
        {
            OAS_DBG << "propname table absence" << OAS_ENDL;
        }
        else
        {
            throw OasisException("propname table offset invalid");
        }
    }
    if (m_layout->propstring_strictmode())
    {
        if(m_layout->m_propstring_offset > 0)
        {
            parse_prop_string_table();
        }
        else if(m_layout->m_propstring_offset == 0)
        {
            OAS_DBG << "propstring table absence" << OAS_ENDL;
        }
        else
        {
            throw OasisException("propstring table offset invalid");
        }
    }
    if (m_layout->layername_strictmode())
    {
        if(m_layout->m_layername_offset > 0)
        {
            parse_layer_name_table();
        }
        else if(m_layout->m_layername_offset == 0)
        {
            OAS_DBG << "layername table absence" << OAS_ENDL;
        }
        else
        {
            throw OasisException("layername table offset invalid");
        }
    }
    if (m_layout->xname_strictmode())
    {
        if(m_layout->m_xname_offset > 0)
        {
            parse_x_name_table();
        }
        else if(m_layout->m_xname_offset == 0)
        {
            OAS_DBG << "xname table absence" << OAS_ENDL;
        }
        else
        {
            throw OasisException("xname table offset invalid");
        }
    }
    if (m_layout->cellname_strictmode())
    {
        if(m_layout->m_cellname_offset > 0)
        {
            parse_cell_name_table();
        }
        else if(m_layout->m_cellname_offset == 0)
        {
            OAS_DBG << "cellname table absence" << OAS_ENDL;
        }
        else
        {
            throw OasisException("cellname table offset invalid");
        }
    }
}

void OasisParser::parse_cell_name_table()
{
    m_reader.seek(m_layout->m_cellname_offset);

    m_modal_variables.reset();
    cache_id(false);

    bool done = false;
    while(!done)
    {
        OasisRecordId rid = read_record_id();
        OAS_DBG << "Record " << oasis_record_id_name(rid) << OAS_ENDL;
        switch(rid)
        {
            case ORID_Pad:
                break;
            case ORID_Cellname:
            case ORID_CellnameRef:
                parse_cell_name(rid);
                break;
            case ORID_Cblock:
                enter_cblock(rid);
                break;
            default:
                done = true;
                cache_id();
                break;
        }
    }
}

void OasisParser::parse_text_string_table()
{
    m_reader.seek(m_layout->m_textstring_offset);

    m_modal_variables.reset();
    cache_id(false);

    bool done = false;
    while(!done)
    {
        OasisRecordId rid = read_record_id();
        OAS_DBG << "Record " << oasis_record_id_name(rid) << OAS_ENDL;
        switch(rid)
        {
            case ORID_Pad:
                break;
            case ORID_Textstring:
            case ORID_TextstringRef:
                parse_text_string(rid);
                break;
            case ORID_Cblock:
                enter_cblock(rid);
                break;
            default:
                done = true;
                cache_id();
                break;
        }
    }
}

void OasisParser::parse_prop_name_table()
{
    m_reader.seek(m_layout->m_propname_offset);

    m_modal_variables.reset();
    cache_id(false);

    bool done = false;
    while(!done)
    {
        OasisRecordId rid = read_record_id();
        OAS_DBG << "Record " << oasis_record_id_name(rid) << OAS_ENDL;
        switch(rid)
        {
            case ORID_Pad:
                break;
            case ORID_Propname:
            case ORID_PropnameRef:
                parse_prop_name(rid);
                break;
            case ORID_Cblock:
                enter_cblock(rid);
                break;
            default:
                done = true;
                cache_id();
                break;
        }
    }
}

void OasisParser::parse_prop_string_table()
{
    m_reader.seek(m_layout->m_propstring_offset);

    m_modal_variables.reset();
    cache_id(false);

    bool done = false;
    while(!done)
    {
        OasisRecordId rid = read_record_id();
        OAS_DBG << "Record " << oasis_record_id_name(rid) << OAS_ENDL;
        switch(rid)
        {
            case ORID_Pad:
                break;
            case ORID_Propstring:
            case ORID_PropstringRef:
                parse_prop_string(rid);
                break;
            case ORID_Cblock:
                enter_cblock(rid);
                break;
            default:
                done = true;
                cache_id();
                break;
        }
    }
}

void OasisParser::parse_layer_name_table()
{
    m_reader.seek(m_layout->m_layername_offset);

    m_modal_variables.reset();
    cache_id(false);

    bool done = false;
    while(!done)
    {
        OasisRecordId rid = read_record_id();
        OAS_DBG << "Record " << oasis_record_id_name(rid) << OAS_ENDL;
        switch(rid)
        {
            case ORID_Pad:
                break;
            case ORID_Layername:
            case ORID_LayernameText:
                parse_layer_name(rid);
                break;
            case ORID_Cblock:
                enter_cblock(rid);
                break;
            default:
                done = true;
                cache_id();
                break;
        }
    }
}

void OasisParser::parse_x_name_table()
{
    m_reader.seek(m_layout->m_xname_offset);

    m_modal_variables.reset();
    cache_id(false);

    bool done = false;
    while(!done)
    {
        OasisRecordId rid = read_record_id();
        OAS_DBG << "Record " << oasis_record_id_name(rid) << OAS_ENDL;
        switch(rid)
        {
            case ORID_Pad:
                break;
            case ORID_XName:
            case ORID_XNameRef:
                parse_x_name(rid);
                break;
            case ORID_Cblock:
                enter_cblock(rid);
                break;
            default:
                done = true;
                cache_id();
                break;
        }
    }
}

bool OasisParser::check_format()
{
    m_reader.init(m_openfp);
    try
    {
        parse_magic();
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool OasisParser::is_generated_cellname(const std::string& name)
{
    size_t j = 0;
    while (name[j] == '_')
    {
        ++j;
        if (j == name.size())
        {
            return false;
        }
    }
    if (name.size() < j + generate_cellname_prefix.size())
    {
        return false;
    }
    for (size_t i = 0; i < generate_cellname_prefix.size(); ++i)
    {
        if (name[j + i] != generate_cellname_prefix[i])
        {
            return false;
        }
    }
    return true;
}

std::string OasisParser::rename_generated_prefix(const std::string& name)
{
    return "_" + name;
}

