#include "gds_parser.h"
#include "oasis_exception.h"
#include "oasis_layout.h"
#include "thread_worker.h"

using namespace std;
using namespace oasis;

void GDSRecord::read(GDSReader& reader)
{
    m_offset = reader.get_data_offset();
    m_record_len = reader.read_uint16();
    m_record_id = static_cast<GDSRecordId>(reader.read_uint16());

    if(m_record_len < 4)
    {
        throw OasisException("invalid record length, < 4");
    }
    if(m_record_len % 2 == 1)
    {
        throw OasisException("invalid record length, odd");
    }

    m_record_len -= 4;

    if(m_record_len > 0)
    {
        reader.read_bytes(m_buffer, m_record_len);
    }
    m_current = m_buffer;
    m_end = m_buffer + m_record_len;
}

std::string GDSRecord::get_string()
{
    if(m_end - m_current == 0)
    {
        return "";
    }

    std::string res;
    if(*(m_end - 1) == 0)
    {
        res.assign(reinterpret_cast<char*>(m_current));
    }
    else
    {
        res.assign(reinterpret_cast<char*>(m_current), m_end - m_current);
    }
    m_current = m_end;
    return res;
}

uint16 GDSRecord::get_uint16()
{
    if(m_end - m_current < 2)
    {
        throw OasisException("insufficient data");
    }
    uint16 res;
    res = *m_current++;
    res <<= 8;
    res |= *m_current++;
    return res;
}

uint32 GDSRecord::get_uint32()
{
    if(m_end - m_current < 4)
    {
        throw OasisException("insufficient data");
    }
    uint32 res;
    res = *m_current++;
    res <<= 8;
    res |= *m_current++;
    res <<= 8;
    res |= *m_current++;
    res <<= 8;
    res |= *m_current++;
    return res;
}

int16 GDSRecord::get_int16()
{
    uint16 res = get_uint16();
    return *reinterpret_cast<int16*>(&res);
}

int32 GDSRecord::get_int32()
{
    uint32 res = get_uint32();
    return *reinterpret_cast<int32*>(&res);
}

float64 GDSRecord::get_float64()
{
    uint32 l0 = get_uint32();
    uint32 l1 = get_uint32();

    uint8 b = (l0 >> 24);
    l0 &= 0xffffff;
    float64 res = 4294967296.0 * float64 (l0) + float64 (l1);

    if(b & 0x80)
    {
        res = -res;
    }

    int16 e = int16(b & 0x7f) - (64 + 14);
    if (e != 0)
    {
        res *= pow(16.0, float64(e));
    }

    return res;
}

void GDSRecord::get_xy(std::vector<Point>& points)
{
    uint32 len = (m_end - m_current) / 8;
    points.reserve(len);
    for(uint32 i = 0; i < len; ++i)
    {
        int32 x = get_int32();
        int32 y = get_int32();
        points.push_back(Point(x, y));
    }
}

GDSParser::GDSParser() : m_parse_mode(FIRST_PASS)
{
    m_use_cached_record = false;
    m_fp = NULL;
}

GDSParser::~GDSParser()
{
    if(m_fp)
    {
        fclose(m_fp);
    }
}

void GDSParser::open_file(const std::string& filename)
{
    m_fp = fopen(filename.c_str(), "rb");
    if(m_fp == NULL)
    {
        OAS_ERR << "open file " << filename << " failed" << OAS_ENDL;
        throw OasisException("open file failed");
    }
    m_filename = filename;
    m_reader.set_fp(m_fp);
    m_reader.set_file_offset(0);
}

void GDSParser::set_file_offset(int64 offset, int64 hint_size)
{
    m_reader.set_file_offset(offset);
    m_reader.reset_buf_size(hint_size);
}

void GDSParser::parse_file(OasisLayout* layout)
{
    OAS_INFO << "begin parse file" << OAS_ENDL;

    oas_assert(layout != NULL);
    m_layout = layout;
    m_layout->cleanup();

    parse_start();

    if (m_parse_option.parse_over())
    {
        parse_all();
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

void GDSParser::parse_start()
{
    GDSRecordId rid = read_record();
    if(rid != GRID_Header)
    {
        throw OasisException("HEADER expected");
    }
    rid = read_record();
    if(rid != GRID_BgnLib)
    {
        throw OasisException("Begin Lib expected");
    }

    std::string libname;
    float64 dbu = -1;

    bool done = false;
    while(!done)
    {
        rid = read_record();
        OAS_DBG << "GDS Record " << gds_record_id_name(rid) << " in parse start" << OAS_ENDL;
        switch(rid)
        {
            case GRID_LibDirSize:
            case GRID_SrfName:
            case GRID_RefLibs:
            case GRID_Fonts:
            case GRID_AttrTable:
            case GRID_Generations:
            case GRID_Format:
            case GRID_Mask:
            case GRID_EndMasks:
                break;
            case GRID_BgnStr:
            case GRID_EndLib:
                cached_record();
                done = true;
                break;
            case GRID_LibName:
                libname = m_record.get_string();
                break;
            case GRID_Units:
                m_record.get_float64();
                dbu = m_record.get_float64() * 1e6;
                break;
            default:
                throw OasisException("unexpected record");
                break;
        }
    }

    OAS_DBG << "libname " << libname << "dbu " << dbu << OAS_ENDL;
    m_layout->set_dbu(dbu);
}

void GDSParser::parse_all()
{
    OAS_INFO << "begin parse all cell " << OAS_ENDL;
    bool done = false;
    while(!done)
    {
        GDSRecordId rid = read_record();
        OAS_DBG << "GDS Record " << gds_record_id_name(rid) << " in parse all" << OAS_ENDL;
        switch(rid)
        {
            case GRID_EndLib:
                cached_record();
                done = true;
                break;
            case GRID_BgnStr:
            {
                int64 cell_index = m_layout->create_cell();
                OasisCell& cell = m_layout->get_cell(cell_index);
                int64 offset = m_record.get_offset();
                cell.set_offset(offset);
                parse_cell(cell_index);
                cell.set_hint_datasize(m_reader.get_data_offset() - offset);
                break;
            }
            default:
                throw OasisException("unexpected record");
                break;
        }
    }
    OAS_INFO << "end parse all cell" << OAS_ENDL;
}

void GDSParser::parse_cell_concurrent(int64 info_index)
{
    OasisCellInfo &info = m_layout->get_cell_info(info_index);
    OasisCell &cell = m_layout->get_cell(info.get_cell_index());
    GDSParser parser;
    parser.set_parse_option(m_parse_option);
    parser.open_file(m_filename);
    parser.set_file_offset(cell.get_offset(), cell.get_hint_datasize());
    parser.set_layout(m_layout);
    parser.parse_cell_second_pass(info.get_cell_index());
}

void GDSParser::parse_cell_second_pass(int64 cell_index)
{
    m_parse_mode = SECOND_PASS;
    GDSRecordId rid = read_record();
    oas_assert(rid == GRID_BgnStr);
    parse_cell(cell_index);
}

void GDSParser::parse_end()
{
    GDSRecordId rid = read_record();
    if(rid != GRID_EndLib)
    {
        throw OasisException("expect end record");
    }
}

void GDSParser::parse_cell(int64 cell_index)
{
    GDSRecordId rid = read_record();
    oas_assert(rid == GRID_StrName);

    if (m_parse_mode == FIRST_PASS)
    {
        std::string cellname = m_record.get_string();
        OAS_DBG << "GDS Record " << gds_record_id_name(rid) << " cellname: " << cellname << OAS_ENDL;
        if (m_layout->m_defined_cells_by_name.find(cellname) != m_layout->m_defined_cells_by_name.end())
        {
            throw OasisException("redefine cell by name " + cellname);
        }
        m_layout->m_defined_cells_by_name.insert(cellname);
        if (m_layout->m_cellname2index.find(cellname) == m_layout->m_cellname2index.end())
        {
            m_layout->m_cellname2index[cellname] = m_layout->create_cell_info(-1, cellname);
        }
        int64 info_index = m_layout->m_cellname2index[cellname];
        OasisCell &cell = m_layout->get_cell(cell_index);
        OasisCellInfo &info = m_layout->get_cell_info(info_index);
        cell.set_infoindex(info_index);
        info.set_cell_index(cell_index);
    }

    OasisCell& cell = m_layout->get_cell(cell_index);

    bool done = false;
    while(!done)
    {
        rid = read_record();
        OAS_DBG << "GDS Record " << gds_record_id_name(rid) << " in parse cell" << OAS_ENDL;
        switch(rid)
        {
            case GRID_StrClass:
                break;
            case GRID_EndStr:
                done = true;
                break;
            case GRID_Boundary:
                read_boundary(cell);
                break;
            case GRID_Path:
                read_path(cell);
                break;
            case GRID_Sref:
                read_ref(cell, false);
                break;
            case GRID_Aref:
                read_ref(cell, true);
                break;
            case GRID_Text:
                read_text(cell);
                break;
            case GRID_Node:
                read_node(cell);
                break;
            case GRID_Box:
                read_box(cell);
                break;
            default:
                throw OasisException("unexpected record");
                break;
        }
    }

    if (m_parse_option.parse_over())
    {
        return;
    }

    if(m_parse_option.do_partition())
    {
        cell.partition(*m_layout);
    }
    cell.try_compute_deep_information(*m_layout);
    cell.unload(*m_layout);
}

void GDSParser::read_boundary(OasisCell& cell)
{
    GDSRecordId rid = read_record();
    if(rid == GRID_Eflags)
    {
        rid = read_record();
    }
    if(rid == GRID_Plex)
    {
        rid = read_record();
    }

    oas_assert(rid == GRID_Layer);
    int16 layer = m_record.get_int16();

    rid = read_record();
    oas_assert(rid == GRID_Datatype);
    int16 datatype = m_record.get_int16();

    rid = read_record();
    oas_assert(rid == GRID_XY);
    std::vector<Point> points;
    if (!m_parse_option.parse_over() && m_parse_option.include_layer(layer, datatype))
    {
        m_record.get_xy(points);
    }

    while((rid = read_record()) == GRID_PropAttr)
    {
        read_property();
    }
    oas_assert(rid == GRID_EndEl);

    if (m_parse_option.parse_over())
    {
        return;
    }

    if (points.size() < 3)
    {
        return;
    }
 
    if(m_parse_option.include_layer(layer, datatype))
    {
        LDType ld = m_parse_option.map_layer(layer, datatype);
        points.pop_back();
        PolygonElement* e = new PolygonElement;
        e->create(points, ld.layer, ld.datatype);
        cell.add_geometry(e);
    }
}

void GDSParser::read_path(OasisCell& cell)
{
    using namespace GDSPathInfoByte;
    GDSRecordId rid = read_record();
    if(rid == GRID_Eflags)
    {
        rid = read_record();
    }
    if(rid == GRID_Plex)
    {
        rid = read_record();
    }

    oas_assert(rid == GRID_Layer);
    int16 layer = m_record.get_int16();

    rid = read_record();
    oas_assert(rid == GRID_Datatype);
    int16 datatype = m_record.get_int16();

    rid = read_record();
    int16 pathtype = 0;
    int32 width = 0;
    int32 bgnextn = 0;
    int32 endextn = 0;
    if(rid == GRID_Pathtype)
    {
        pathtype = m_record.get_int16();
        rid = read_record();
    }
    if(rid == GRID_Width)
    {
        width = m_record.get_int32();
        rid = read_record();
    }
    if(rid == GRID_BgnExtn)
    {
        bgnextn = m_record.get_int32();
        rid = read_record();
    }
    if(rid == GRID_EndExtn)
    {
        endextn = m_record.get_int32();
        rid = read_record();
    }
    oas_assert(rid == GRID_XY);
    std::vector<Point> points;
    if (!m_parse_option.parse_over() && m_parse_option.include_layer(layer, datatype))
    {
        m_record.get_xy(points);
    }

    while((rid = read_record()) == GRID_PropAttr)
    {
        read_property();
    }
    oas_assert(rid == GRID_EndEl);

    if (m_parse_option.parse_over())
    {
        return;
    }

    if(pathtype == FlushExtension)
    {
        bgnextn = endextn = 0;
    }
    else if(pathtype == RoundExtension)
    {
        OAS_WARN << "round extension is converted to square extension" << OAS_ENDL;
        bgnextn = endextn = width / 2;
    }
    else if(pathtype == SquareExtension)
    {
        bgnextn = endextn = width / 2;
    }
    else if(pathtype == ExplicitExtension)
    {
    }
    else
    {
        OAS_WARN << "unknown path type " << pathtype << OAS_ENDL;
        throw OasisException("unknown path type");
    }

    if(m_parse_option.include_layer(layer, datatype))
    {
        LDType ld = m_parse_option.map_layer(layer, datatype);
        PathElement* e = new PathElement;
        e->create(bgnextn, endextn, width / 2, points, ld.layer, ld.datatype);
        cell.add_geometry(e);
    }
}

void GDSParser::read_ref(OasisCell& cell, bool array)
{
    using namespace StransInfoByte;
    GDSRecordId rid = read_record();
    if(rid == GRID_Eflags)
    {
        rid = read_record();
    }
    if(rid == GRID_Plex)
    {
        rid = read_record();
    }

    oas_assert(GRID_Sname);
    std::string cellname = m_record.get_string();
    int64 infoindex;
    if(m_layout->m_cellname2index.find(cellname) != m_layout->m_cellname2index.end())
    {
        infoindex = m_layout->m_cellname2index[cellname];
    }
    else
    {
        infoindex = m_layout->create_cell_info(-1, cellname);
        m_layout->m_cellname2index[cellname] = infoindex;
    }
    OasisCellInfo& info = m_layout->get_cell_info(infoindex);
    info.set_referenced();

    if (m_parse_mode == FIRST_PASS)
    {
        m_layout->m_info2children[cell.get_infoindex()].insert(infoindex);
        m_layout->m_info2parents[infoindex].insert(cell.get_infoindex());
    }

    rid = read_record();
    uint16 strans = 0;
    float64 mag = 1.0;
    bool is_mag = false;
    float64 angle = 0.0;
    bool anyangle = false;
    if(rid == GRID_Strans)
    {
        strans = m_record.get_uint16();

        rid = read_record();
        if(rid == GRID_Mag)
        {
            mag = m_record.get_float64();
            if(fabs(mag - 1.0) > 1e-6)
            {
                is_mag = true;
            }
            rid = read_record();
        }
        if(rid == GRID_Angle)
        {
            angle = m_record.get_float64();
            anyangle = true;
            rid = read_record();
        }
    }

    int16 cols = 0;
    int16 rows = 0;
    if(array)
    {
        oas_assert(rid == GRID_ColRow);
        cols = m_record.get_int16();
        rows = m_record.get_int16();
        rid = read_record();
    }

    oas_assert(rid == GRID_XY);
    std::vector<Point> points;
    m_record.get_xy(points);

    while((rid = read_record()) == GRID_PropAttr)
    {
        read_property();
    }
    oas_assert(rid == GRID_EndEl);

    if (m_parse_option.parse_over())
    {
        return;
    }

    bool mirror = false;
    if(strans & HasMirror)
    {
        mirror = true;
    }
    if(strans & HasAbsMag)
    {
        throw OasisException("not support abs mag");
    }
    if(strans & HasAbsAngle)
    {
        throw OasisException("not support abs angle");
    }
    uint8 rot = 0;
    if(anyangle)
    {
        if(angle > 360 || angle < -360)
        {
            OAS_WARN << "angle out of range " << angle << OAS_ENDL;
        }
        angle -= floor(angle / 360.0) * 360.0;
        float64 round = rint(angle / 90.0) * 90.0;
        if(fabs(round - angle) < 1e-6)
        {
            anyangle = false;
            rot = (uint8)rint(angle / 90.0);
        }
    }

    OasisTrans trans;
    if(!is_mag && !anyangle)
    {
        trans = OasisTrans(mirror, rot, points[0]);
    }
    else
    {
        trans = OasisTrans(mirror, angle, mag, points[0]);
    }

    if(array)
    {
        Point colvec = points[1] - points[0];
        Point rowvec = points[2] - points[0];
        bool split_col = false;
        bool split_row = false;
        if(cols > 1 && (colvec.x() % cols != 0 || colvec.y() % cols != 0))
        {
            split_col = true;
        }
        if(rows > 1 && (rowvec.x() % rows != 0 || rowvec.y() % rows != 0))
        {
            split_row = true;
        }

        if(!split_col && !split_row)
        {
            colvec.set_x(colvec.x() / cols);
            colvec.set_y(colvec.y() / cols);
            rowvec.set_x(rowvec.x() / rows);
            rowvec.set_y(rowvec.y() / rows);

            add_array(cell, infoindex, trans, cols, rows, colvec, rowvec);
        }
        else
        {
            if(cols < 1 || rows < 1)
            {
                throw OasisException("error col/row");
            }
            PointF colvecf((float64)colvec.x() / cols, (float64)colvec.y() / cols);
            PointF rowvecf((float64)rowvec.x() / rows, (float64)rowvec.y() / rows);

            colvec.set_x((int64)rint(colvecf.x()));
            colvec.set_y((int64)rint(colvecf.y()));
            rowvec.set_x((int64)rint(rowvecf.x()));
            rowvec.set_y((int64)rint(rowvecf.y()));

            int ic = 0;
            while(ic < cols)
            {
                int ic0 = ic;

                if(!split_col)
                {
                    ic = cols;
                }
                else
                {
                    PointF colxyf(colvecf.x() * ic, colvecf.y() * ic);
                    Point colxy((int64)rint(colxyf.x()), (int64)rint(colxyf.y()));

                    do
                    {
                        ++ic;
                        colxyf = colxyf + colvecf;
                        colxy = colxy + colvec;
                    }while(ic < cols && fabs(colxyf.x() - colxy.x()) < 0.5 && fabs(colxyf.y() - colxy.y()) < 0.5);

                }

                int ir = 0;
                while(ir < rows)
                {
                    int ir0 = ir;

                    if(!split_row)
                    {
                        ir = rows;
                    }
                    else
                    {
                        PointF rowxyf(rowvecf.x() * ir, rowvecf.y() * ir);
                        Point rowxy((int64)rint(rowvecf.x()), (int64)rint(rowvecf.y()));

                        do
                        {
                            ++ir;
                            rowxyf = rowxyf + rowvecf;
                            rowxy = rowxy + rowvec;
                        }while(ir < rows && fabs(rowxyf.x() - rowxy.x()) < 0.5 && fabs(rowxyf.y() - rowxy.y()) < 0.5);

                    }

                    Point offset(
                                    (int64)rint(colvecf.x() * ic0 + rowvecf.x() * ir0),
                                    (int64)rint(colvecf.y() * ic0 + rowvecf.y() * ir0)
                                    );

                    add_array(cell, infoindex, trans.shifted(offset), ic - ic0, ir - ir0, colvec, rowvec);
                }
            }
        }
    }
    else
    {
        PlacementElement* p = new PlacementElement;
        p->create(infoindex, trans);
        cell.add_instance(p);
    }
}

void GDSParser::add_array(OasisCell& cell, int64 infoindex, const OasisTrans &trans, int64 cols, int64 rows, const Point &cv, const Point &rv)
{
    if(cols == 1 && rows == 1)
    {
        PlacementElement* p = new PlacementElement;
        p->create(infoindex, trans);
        cell.add_instance(p);
        return;
    }

    OasisRepetition repetition;
    OasisRepetitionBuilder builder;
    if(cols == 1)
    {
        builder.make_diagonal_1d(repetition, rows, OasisDelta(rv.x(), rv.y()));
    }
    else if(rows == 1)
    {
        builder.make_diagonal_1d(repetition, cols, OasisDelta(cv.x(), cv.y()));
    }
    else
    {
        builder.make_diagonal_2d(repetition, cols, OasisDelta(cv.x(), cv.y()), rows, OasisDelta(rv.x(), rv.y()));
    }
    std::vector<OasisRepetition> repetitions;
    std::vector<Point> offsets;
    if(builder.partition(repetition, m_layout->repetition_dimension_limit(), repetitions, offsets))
    {
        for(uint32 i = 0; i < repetitions.size(); ++i)
        {
            PlacementElement* p = new PlacementElement;
            OasisRepetition& r = repetitions[i];
            Point& os = offsets[i];
            p->create(
                infoindex,
                trans.shifted(os),
                (r.valid() ? &r : NULL)
                );
            cell.add_instance(p);
        }
    }
    else
    {
        PlacementElement* p = new PlacementElement;
        p->create(
            infoindex,
            trans,
            &repetition
            );
        cell.add_instance(p);
    }
}

void GDSParser::read_text(OasisCell& cell)
{
    using namespace StransInfoByte;
    GDSRecordId rid = read_record();
    if(rid == GRID_Eflags)
    {
        rid = read_record();
    }
    if(rid == GRID_Plex)
    {
        rid = read_record();
    }

    oas_assert(rid == GRID_Layer);

    rid = read_record();
    oas_assert(rid == GRID_Texttype);

    rid = read_record();
    if(rid == GRID_Presentation)
    {
        rid = read_record();
    }
    if(rid == GRID_Pathtype)
    {
        rid = read_record();
    }
    if(rid == GRID_Width)
    {
        rid = read_record();
    }

    if(rid == GRID_Strans)
    {
        rid = read_record();
        if(rid == GRID_Mag)
        {
            rid = read_record();
        }
        if(rid == GRID_Angle)
        {
            rid = read_record();
        }
    }
    oas_assert(rid == GRID_XY);

    rid = read_record();
    oas_assert(rid == GRID_String);

    while((rid = read_record()) == GRID_PropAttr)
    {
        read_property();
    }
    oas_assert(rid == GRID_EndEl);

    if (m_parse_option.parse_over())
    {
        return;
    }

}

void GDSParser::read_node(OasisCell& cell)
{
    GDSRecordId rid = read_record();
    if(rid == GRID_Eflags)
    {
        rid = read_record();
    }
    if(rid == GRID_Plex)
    {
        rid = read_record();
    }

    oas_assert(rid == GRID_Layer);
    rid = read_record();
    oas_assert(rid == GRID_Nodetype);
    rid = read_record();
    oas_assert(rid == GRID_XY);
    while((rid = read_record()) == GRID_PropAttr)
    {
        read_property();
    }
    oas_assert(rid == GRID_EndEl);

    if (m_parse_option.parse_over())
    {
        return;
    }
}

void GDSParser::read_box(OasisCell& cell)
{
    GDSRecordId rid = read_record();
    if(rid == GRID_Eflags)
    {
        rid = read_record();
    }
    if(rid == GRID_Plex)
    {
        rid = read_record();
    }

    oas_assert(rid == GRID_Layer);
    int16 layer = m_record.get_int16();

    rid = read_record();
    oas_assert(rid == GRID_Boxtype);
    int16 boxtype = m_record.get_int16();

    rid = read_record();
    oas_assert(rid == GRID_XY);
    std::vector<Point> points;
    if(m_parse_option.include_layer(layer, boxtype))
    {
        m_record.get_xy(points);
    }

    while((rid = read_record()) == GRID_PropAttr)
    {
        read_property();
    }
    oas_assert(rid == GRID_EndEl);

    if (m_parse_option.parse_over())
    {
        return;
    }

    Box box;
    for(uint32 i = 0; i < points.size(); ++i)
    {
        box.update(points[i]);
    }

    if(m_parse_option.include_layer(layer, boxtype))
    {
        LDType ld = m_parse_option.map_layer(layer, boxtype);
        RectangleElement* e = new RectangleElement;
        e->create(box, ld.layer, ld.datatype);
        cell.add_geometry(e);
    }
}

void GDSParser::read_property()
{
    GDSRecordId rid = read_record();
    oas_assert(rid == GRID_PropValue);
    (void)rid;
}

GDSRecordId GDSParser::read_record()
{
    if(m_use_cached_record)
    {
        m_use_cached_record = false;
    }
    else
    {
        m_record.read(m_reader);
    }

    return m_record.get_record_id();
}

bool GDSParser::check_format()
{
    OasisLayout tmplayout("");
    m_layout = &tmplayout;
    try
    {
        parse_start();
    }
    catch(oasis::OasisException& e)
    {
        return false;
    }

    return true;
}
