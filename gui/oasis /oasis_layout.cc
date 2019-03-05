#include "oasis_layout.h"
#include "oasis_util.h"
#include "oasis_exception.h"
#include "cell_visitor.h"
#include "fetch_polygons_visitor.h"
#include "fetch_instance_visitor.h"
#include "count_mark_visitor.h"
#include "select_polygons_visitor.h"
#include "draw_bitmap_visitor.h"
#include "snap_edge_visitor.h"
#include <sstream>
#include <time.h>
#include "thread_worker.h"

using namespace std;
using namespace oasis;

OasisLayout::OasisLayout(const std::string& filename)
{
    m_major_version = 1;
    m_minor_version = 0;

    for (uint32 i = 0; i < 5; ++i) m_sourcefile_sha1[i] = 0;
    
    m_filename = filename;
    m_loadfp = NULL;
    m_writefp = NULL;
    m_multiple_thread_write = false;
    m_compress_geometry = true;
}

OasisLayout::~OasisLayout()
{
    cleanup();
    if(m_loadfp)
    {
        fclose(m_loadfp);
    }
    if(m_writefp)
    {
        fclose(m_writefp);
    }
}

void OasisLayout::open_file(bool w)
{
    if(w)
    {
        m_writefp = fopen(m_filename.c_str(), "w");
        if(!m_writefp)
        {
            throw OasisException("open file " + m_filename + " for write failed");
        }
        m_writer.set_file_offset(m_writefp, 0);
    }

    m_loadfp = fopen(m_filename.c_str(), "r");
    if(!m_loadfp)
    {
        throw OasisException("open file " + m_filename + " for read failed");
    }
}

void OasisLayout::write_start(OasisWriter& writer)
{
    writer.write_record_id(ORID_Start);
    writer.write_string("1.0");
    writer.write_real(OasisReal(1.0 / m_dbu));
    writer.write_byte(1); //offset at end;

    ModalVariable& mv = writer.get_modal_variables();
    mv.reset();

    std::vector<OasisProperty> properties;
    get_file_properties(properties);
    for (size_t i = 0; i < properties.size(); ++i)
    {
        properties[i].write(writer);
    }
}

void OasisLayout::write_tables(OasisWriter& writer)
{
    OAS_INFO << "begin write tables " << OAS_ENDL;

    m_cellname_flag = true;
    m_cellname_offset = 0;
    if (m_cellinfos.size())
    {
        m_cellname_offset = writer.get_offset();
        writer.enter_cblock();
        for(uint32 i = 0; i < m_cellinfos.size(); ++i)
        {
            ModalVariable& mv = writer.get_modal_variables();
            mv.reset();
            OasisCellInfo& info = m_cellinfos[i];
            writer.write_record_id(ORID_CellnameRef);
            writer.write_string(info.get_name());

            OasisCell& cell = get_cell(info.get_cell_index());
            writer.write_uint(cell.get_infoindex());

            std::vector<OasisProperty> properties;
            info.get_properties(*this, properties);
            for (size_t i = 0; i < properties.size(); ++i)
            {
                properties[i].write(writer);
            }

            OAS_DBG << "write " << oasis_record_id_name(ORID_CellnameRef) << " " << info.get_name() << " " << cell.get_infoindex() << OAS_ENDL;
        }
        writer.leave_cblock();
        OAS_DBG << "write cellname offset " << m_cellname_offset << OAS_ENDL;
    }

    m_textstring_flag = true;
    m_textstring_offset = 0;
    if (m_textstrings.size())
    {
        m_textstring_offset = writer.get_offset();
        writer.enter_cblock();
        for(uint32 i = 0; i < m_textstrings.size(); ++i)
        {
            OasisTextString& textstring = m_textstrings[i];
            writer.write_record_id(ORID_TextstringRef);
            writer.write_string(textstring.get_name());
            writer.write_uint(textstring.get_index());

            OAS_DBG << "write " << oasis_record_id_name(ORID_TextstringRef) << " " << textstring.get_name().value() << " " << textstring.get_index() << OAS_ENDL;
        }
        writer.leave_cblock();
        OAS_DBG << "write textstrings offset " << m_textstring_offset << OAS_ENDL;
    }

    m_propname_flag = true;
    m_propname_offset = 0;
    if (m_output_propnames.size())
    {
        m_propname_offset = writer.get_offset();
        writer.enter_cblock();
        for(uint32 i = 0; i < m_output_propnames.size(); ++i)
        {
            OasisPropName& propname = m_output_propnames[i];
            writer.write_record_id(ORID_PropnameRef);
            writer.write_string(propname.get_name());
            writer.write_uint(propname.get_index());
            OAS_DBG << "write " << oasis_record_id_name(ORID_PropnameRef) << " " << propname.get_name().value() << " " << propname.get_index() << OAS_ENDL;
        }
        writer.leave_cblock();
        OAS_DBG << "write propnames offset " << m_propname_offset << OAS_ENDL;
    }

    m_propstring_flag = true;
    m_propstring_offset = 0;
    if (m_output_propstrings.size())
    {
        m_propstring_offset = writer.get_offset();
        writer.enter_cblock();
        for(uint32 i = 0; i < m_output_propstrings.size(); ++i)
        {
            OasisPropString& propstring = m_output_propstrings[i];
            writer.write_record_id(ORID_PropstringRef);
            writer.write_string(propstring.get_name());
            writer.write_uint(propstring.get_index());
            OAS_DBG << "write " << oasis_record_id_name(ORID_PropstringRef) << " " << propstring.get_name().value() << " " << propstring.get_index() << OAS_ENDL;
        }
        writer.leave_cblock();
        OAS_DBG << "write propstrings offset " << m_propstring_offset << OAS_ENDL;
    }

    m_layername_flag = true;
    m_layername_offset = 0;
    //TODO check textlayername
    if (m_layernames.size())
    {
        m_layername_offset = writer.get_offset();
        writer.enter_cblock();
        for(std::vector<OasisLayerName>::iterator it = m_layernames.begin(); it != m_layernames.end(); ++it)
        {
            OasisLayerName& layername = *it;
            layername.write(writer);
            OAS_DBG << "write " << oasis_record_id_name(ORID_Layername) << " " << layername.get_name() << OAS_ENDL;
        }
        writer.leave_cblock();
        OAS_DBG << "write layername offset " << m_layername_offset << OAS_ENDL;
    }

    m_xname_flag = true;
    m_xname_offset = 0;

    OAS_INFO << "end write tables" << OAS_ENDL;
}

void OasisLayout::flush_writer(OasisWriter& writer)
{
    writer.flush_buffer();
}

void OasisLayout::write_end(OasisWriter& writer)
{
    uint64 end_record_offset = writer.get_offset();
    OAS_DBG << "write end offset " << end_record_offset << OAS_ENDL;

    writer.write_record_id(ORID_End);

    writer.write_uint(m_cellname_flag);
    writer.write_uint(m_cellname_offset);
    writer.write_uint(m_textstring_flag);
    writer.write_uint(m_textstring_offset);
    writer.write_uint(m_propname_flag);
    writer.write_uint(m_propname_offset);
    writer.write_uint(m_propstring_flag);
    writer.write_uint(m_propstring_offset);
    writer.write_uint(m_layername_flag);
    writer.write_uint(m_layername_offset);
    writer.write_uint(m_xname_flag);
    writer.write_uint(m_xname_offset);

    uint64 used_len = writer.get_offset() - end_record_offset;

    for(uint64 i = 0; i < 254 - used_len; ++i)
    {
        writer.write_byte(0x80);
    }
    writer.write_byte(0);

    writer.write_byte(0);

    writer.flush_buffer(true);

    oas_assert(writer.get_offset() == end_record_offset + 256);
}

int64 OasisLayout::create_cell_info(int64 refnum, const std::string& cellname)
{
    sys::MutexGuard guard(m_create_locker);
    int64 index = m_cellinfos.size();
    OasisCellInfo info;
    info.set_ref_num(refnum);
    info.set_name(cellname);
    info.set_alias(-1);
    info.set_cell_index(-1);
    m_cellinfos[index] = info;
    return index;
}

uint64 OasisLayout::create_cell(bool import)
{
    sys::MutexGuard guard(m_create_locker);
    uint64 index = m_cells.size();
    m_cells[index] = new OasisCell(import);
    return index;
}

OasisCell& OasisLayout::get_cell(uint64 index)
{
    sys::MutexGuard guard(m_create_locker);
    oas_assert(index < m_cells.size());
    return *m_cells.at(index);
}

const OasisCell& OasisLayout::get_cell(uint64 index) const
{
    sys::MutexGuard guard(m_create_locker);
    oas_assert(index < m_cells.size());
    return *m_cells.at(index);
}

OasisCellInfo& OasisLayout::get_cell_info(int64 index)
{
    sys::MutexGuard guard(m_create_locker);
    oas_assert((uint64)index < m_cellinfos.size());
    return m_cellinfos.at(index);
}

const OasisCellInfo& OasisLayout::get_cell_info(int64 index) const
{
    sys::MutexGuard guard(m_create_locker);
    oas_assert((uint64)index < m_cellinfos.size());
    return m_cellinfos.at(index);
}

int64 OasisLayout::info_index_by_name(const std::string& name)
{
    for(uint32 i = 0; i < m_cellinfos.size(); ++i)
    {
        OasisCellInfo& info = m_cellinfos[i];
        if(info.get_name() == name)
        {
            return i;
        }
    }
    return -1;
}

void OasisLayout::add_layer_name(int layer, int datatype, const std::string& name)
{
    OasisIntervalBuilder builder;
    OasisInterval layer_interval = builder.make_interval(OasisInterval::OIT_Exact, layer, layer);
    OasisInterval datatype_interval = builder.make_interval(OasisInterval::OIT_Exact, datatype, datatype);
    OasisLayerName layername(name, layer_interval, datatype_interval, false);
    for(size_t i = 0; i < m_layernames.size(); ++i)
    {
        if(layername == m_layernames[i])
        {
            return;
        }
    }
    m_layernames.push_back(layername);
}

void OasisLayout::get_layers(std::set<std::pair<int, int> >& layers) const
{
    layers = m_layers;
}
void OasisLayout::update_layers(const std::set<std::pair<int32, int32> >& layers)
{
    sys::MutexGuard guard(m_layers_locker);
    for (auto it = layers.begin(); it != layers.end(); ++it)
    {
        m_layers.insert(*it);
    }
}

std::string OasisLayout::get_layer_name(int layer, int datatype) const
{
    for (size_t i = 0; i < m_layernames.size(); ++i)
    {
        const OasisLayerName &layername = m_layernames[i];
        if (layername.contains(layer, datatype))
        {
            return layername.get_name();
        }
    }
    return std::string("");
}

void OasisLayout::cleanup()
{
    for (auto it = m_cells.begin(); it != m_cells.end(); ++it)
    {
        delete it->second;
    }
    m_cells.clear();
    m_cellinfos.clear();
    m_file_properties.clear();
    m_defined_cells_by_name.clear();
    m_defined_cells_by_ref.clear();
    m_cellname2index.clear();
    m_cellref2index.clear();
    m_textstrings.clear();
    m_textref2index.clear();
    m_textstring2index.clear();
    m_propnames.clear();
    m_propnameref2index.clear();
    m_propname2index.clear();
    m_propstrings.clear();
    m_propstringref2index.clear();
    m_propstring2index.clear();
    m_layernames.clear();
}

void OasisLayout::collect_top_cells()
{
    m_topcellinfos.clear();
    for(uint32 i = 0; i < m_cellinfos.size(); ++i)
    {
        OasisCellInfo& cellinfo = m_cellinfos[i];
        if(cellinfo.is_defined() && !cellinfo.is_referenced())
        {
            m_topcellinfos.push_back(i);
        }
    }
}

void OasisLayout::resolve_alias()
{
    for(uint32 i = 0; i < m_cellinfos.size(); ++i)
    {
        OasisCellInfo& cellinfo = m_cellinfos[i];
        if(!cellinfo.is_defined())
        {
            OAS_DBG << "undefined cell " << i << " alias " << cellinfo.get_alias() << OAS_ENDL;
            if(cellinfo.get_alias() < 0)
            {
                throw OasisException("undefined cell has no alias");
            }
            OasisCellInfo& aliasinfo = get_cell_info(cellinfo.get_alias());
            if(!aliasinfo.is_defined())
            {
                throw OasisException("undefined alias cell");
            }
            OasisCell& aliascell = get_cell(aliasinfo.get_cell_index());
            int64 cellindex = create_cell();
            OasisCell& cell = get_cell(cellindex);
            aliascell.clone(cell, *this);
            cell.unload(*this);
            cellinfo.set_cell_index(cellindex);
        }
    }
}

void OasisLayout::build_hierarchy()
{
    OAS_INFO << "begin build hierarchy" << OAS_ENDL;
    uint64 total_size = 0;
    for (uint32 i = 0; i < m_cells.size(); ++i)
    {
        OasisCell& cell = *m_cells[i];
        total_size += cell.get_hint_datasize();
    }
    uint32 thread_count = 10;
    std::vector<sys::Thread *> workers;
    uint64 average_size = total_size / thread_count;
    uint32 begin = 0;
    uint32 end = begin;
    uint64 current_size = 0;
    while (end < m_cells.size())
    {
        OasisCell& cell = *m_cells[end++];
        current_size += cell.get_hint_datasize();
        if (current_size >= average_size || end >= m_cells.size())
        {
            sys::Thread *worker = new sys::WorkThread<OasisLayout, std::pair<uint32, uint32> >(*this, &OasisLayout::collect_children, std::make_pair(begin, end));
            worker->SetName("Collect");
            worker->Start();
            workers.push_back(worker);

            current_size = 0;
            begin = end;
        }
    }
    for (uint32 i = 0; i < workers.size(); ++i)
    {
        sys::Thread* worker = workers[i];
        worker->Wait();
        delete worker;
    }
    workers.clear();

    for (uint32 i = 0; i < m_cells.size(); ++i)
    {
        OasisCell& cell = *m_cells[i];
        cell.collect_parents(*this);
    }

    m_top_down_infos.clear ();
    m_top_down_infos.reserve(m_cellinfos.size());

    std::vector<size_t> num_parents(m_cellinfos.size(), 0);

    while (m_top_down_infos.size() != m_cellinfos.size())
    {
        size_t n_top_down_cells = m_top_down_infos.size();

        for (uint32 i = 0; i < m_cellinfos.size(); ++i)
        {
            OasisCellInfo& cell_info = m_cellinfos[i];
            OasisCell &cell = get_cell(cell_info.get_cell_index());
            if (cell.parent_infos().size() == num_parents[i])
            {
                m_top_down_infos.push_back(i);
                num_parents[i] = m_cellinfos.size();
            }
        }

        for(uint32 i = n_top_down_cells; i != m_top_down_infos.size(); ++i)
        {
            int64 info_index = m_top_down_infos[i];
            OasisCellInfo &cell_info = get_cell_info(info_index);
            OasisCell &cell = get_cell(cell_info.get_cell_index());
            const std::set<int64>& child_infos = cell.child_infos();
            for (auto it = child_infos.begin(); it != child_infos.end(); ++it)
            {
                int64 child_info_index = *it;
                oas_assert(num_parents[child_info_index] != m_cellinfos.size());
                num_parents[child_info_index] += 1;
            }
        }

    }

    OAS_INFO << "end build hierarchy" << OAS_ENDL;
}

void OasisLayout::collect_children(std::pair<uint32, uint32> range)
{
    for (uint32 i = range.first; i < range.second; ++i)
    {
        OasisCell& cell = *m_cells[i];
        cell.collect_children(*this);
    }
}

void OasisLayout::print_hierarchy()
{
    for (uint32 i = 0; i < m_cells.size(); ++i)
    {
        OasisCell& cell = *m_cells[i];
        cell.print_children();
    }
    for (uint32 i = 0; i < m_cells.size(); ++i)
    {
        OasisCell& cell = *m_cells[i];
        cell.print_parents();
    }
    for (uint32 i = 0; i < m_top_down_infos.size(); ++i)
    {
        OAS_INFO << i << ", " << m_top_down_infos[i] << OAS_ENDL;
    }
}

void OasisLayout::get_all_leaves(std::set<int32>& leaves)
{
    leaves.clear();
    for (uint32 i = 0; i < m_cellinfos.size(); ++i)
    {
        OasisCellInfo& info = m_cellinfos[i];
        OasisCell &cell = get_cell(info.get_cell_index());
        if (cell.child_infos().empty())
        {
            leaves.insert(cell.get_infoindex());
        }
    }
}

void OasisLayout::get_all_instances(int64 infoindex, const std::set<int32>& leaves, std::map<int32, std::vector<OasisTrans> >& instances)
{
    if (infoindex < 0)
    {
        for(uint32 i = 0; i < m_topcellinfos.size(); ++i)
        {
            get_all_instances(m_topcellinfos[i], leaves, instances);
        }
        return;
    }

    update_time();
    OasisCellInfo& cellinfo = get_cell_info(infoindex);
    OasisCell& cell = get_cell(cellinfo.get_cell_index());
    FetchInstanceVisitor visitor(leaves, instances);
    visitor.set_layout(this);
    visitor.set_top_query_box(cell.get_bbox());
    visitor.set_top_trans(OasisTrans());
    cell.accept_visitor(visitor);
}

void OasisLayout::load_all_cell()
{
    OAS_DBG << "layout load " << OAS_ENDL;
    for(uint32 i = 0; i < m_cellinfos.size(); ++i)
    {
        OasisCellInfo& info = m_cellinfos[i];
        oas_assert(info.is_defined());
        OasisCell& cell = get_cell(info.get_cell_index());
        OAS_DBG << "start load cell " << i << OAS_ENDL;
        cell.load(*this);
        OAS_DBG << "end load cell" << OAS_ENDL;
    }
}

void OasisLayout::unload(bool force, int expire)
{
    OAS_INFO << "begin layout unload" << OAS_ENDL;
    update_time();
    for(uint32 i = 0; i < m_cellinfos.size(); ++i)
    {
        OasisCellInfo& cellinfo = m_cellinfos[i];
        if(!cellinfo.is_defined())
        {
            if(force)
            {
                throw OasisException("unload undefined cell");
            }
            else
            {
                continue;
            }
        }
        OasisCell& cell = get_cell(cellinfo.get_cell_index());
        if (expire <= 0 || cell.use_time() + expire < get_time())
        {
            OAS_DBG << "start unload cell " << i << OAS_ENDL;
            cell.unload(*this);
            OAS_DBG << "end unload cell " << i << OAS_ENDL;
        }
    }
    OAS_INFO << "end layout unload" << OAS_ENDL;
}

uint64 OasisLayout::count_objects()
{
    uint64 objcount = 0;
    for(uint32 i = 0; i < m_cellinfos.size(); ++i)
    {
        OasisCellInfo& cellinfo = m_cellinfos[i];
        if(cellinfo.is_defined())
        {
            OasisCell& cell = get_cell(cellinfo.get_cell_index());
            objcount += cell.count_objects();
        }
    }
    return objcount;
}

Box OasisLayout::get_bbox()
{
    Box bbox;
    for(uint32 i = 0; i < m_topcellinfos.size(); ++i)
    {
        int64 index = m_topcellinfos[i];
        OasisCellInfo& cellinfo = get_cell_info(index);
        OasisCell& cell = get_cell(cellinfo.get_cell_index());
        bbox.update(cell.get_bbox());
    }
    return bbox;
}

Box OasisLayout::cell_bbox(int64 index)
{
    OasisCellInfo& cellinfo = get_cell_info(index);
    OasisCell& cell = get_cell(cellinfo.get_cell_index());
    return cell.get_bbox();
}

std::string OasisLayout::cell_name(int64 index)
{
    OasisCellInfo& cellinfo = get_cell_info(index);
    return cellinfo.get_name();
}

void OasisLayout::get_polygons(int64 infoindex, Box query_box, OasisTrans trans, const LDType& ld, std::vector<Polygon>& results)
{
    get_polygons_with_attachments(infoindex, query_box, trans, ld, &results, 0);
}

void OasisLayout::get_polygons_with_attachments(
    int64 infoindex,
    Box query_box,
    OasisTrans trans,
    const LDType &ld,
    std::vector<Polygon> *results,
    std::vector<int32> *attach_keys)
{
    if (infoindex < 0)
    {
        for(uint32 i = 0; i < m_topcellinfos.size(); ++i)
        {
            get_polygons_with_attachments(m_topcellinfos[i], query_box, trans, ld, results, attach_keys);
        }
        return;
    }

    update_time();
    OasisCellInfo& cellinfo = get_cell_info(infoindex);
    OasisCell& cell = get_cell(cellinfo.get_cell_index());
    FetchPolygonsVisitor visitor(ld, results, attach_keys);
    visitor.set_layout(this);
    visitor.set_top_query_box(query_box.intersected(cell.get_bbox()));
    visitor.set_top_trans(trans);
    cell.accept_visitor(visitor);
}

void OasisLayout::select_polygons(int64 top_index, int64 select_index, const LDType& ld, std::vector<Polygon>& results)
{
    if (top_index < 0)
    {
        for(uint32 i = 0; i < m_topcellinfos.size(); ++i)
        {
            select_polygons(m_topcellinfos[i], select_index, ld, results);
        }
        return;
    }

    update_time();
    OasisCellInfo &cellinfo = get_cell_info(top_index);
    OasisCell& cell = get_cell(cellinfo.get_cell_index());
    SelectPolygonsVisitor visitor(select_index, ld, results);
    visitor.set_layout(this);
    visitor.set_top_query_box(Box::world());
    visitor.set_top_trans(OasisTrans());
    visitor.set_selected(top_index == select_index);
    cell.accept_visitor(visitor);
}

int32 OasisLayout::count_mark(int64 top_index, const std::string& subname, std::map<std::string, std::string> &parent_to_subcell, std::map<std::string, int> &corner_to_count)
{
    if (top_index < 0)
    {
        int32 count = 0;
        for(uint32 i = 0; i < m_topcellinfos.size(); ++i)
        {
            count += count_mark(m_topcellinfos[i], subname, parent_to_subcell, corner_to_count);
        }
        return count;
    }

    update_time();
    OasisCellInfo &cellinfo = get_cell_info(top_index);
    OasisCell& cell = get_cell(cellinfo.get_cell_index());
    CountMarkVisitor visitor(subname, parent_to_subcell, corner_to_count);
    visitor.set_layout(this);
    visitor.set_top_query_box(Box::world());
    visitor.set_top_trans(OasisTrans());
    visitor.set_parent_name(cellinfo.get_name());
    cell.accept_visitor(visitor);
    return visitor.count();
}

void OasisLayout::draw_bitmap(
    int64 infoindex,
    Box query_box,
    OasisTrans trans,
    const LDType &ld,
    uint32 width,
    uint32 height,
    float64 resolution,
    render::Bitmap* contour,
    render::Bitmap* fill,
    render::Bitmap* vertex)
{
    if (infoindex < 0)
    {
        for(uint32 i = 0; i < m_topcellinfos.size(); ++i)
        {
            draw_bitmap(m_topcellinfos[i], query_box, trans, ld, width, height, resolution, contour, fill, vertex);
        }
        return;
    }

    update_time();
    OasisCellInfo& cellinfo = get_cell_info(infoindex);
    OasisCell& cell = get_cell(cellinfo.get_cell_index());
    DrawBitmapVisitor visitor(ld, width, height, resolution,contour, fill, vertex);
    visitor.set_layout(this);
    visitor.set_top_query_box(query_box.intersected(cell.get_bbox()));

    visitor.set_top_trans(trans);

    cell.accept_visitor(visitor);
}

void OasisLayout::snapping_edge(
    int64 infoindex,
    Box query_box,
    OasisTrans trans,
    const LDType & ld,
    const PointF& p,
    const BoxF& micron_box,
    PointF* snap_point,
    EdgeF* snap_edge,
    double* distance,
    bool* find_any,
    int mode)
{
    if(infoindex < 0)
    {
        for(uint32 i = 0; i < m_topcellinfos.size(); ++i)
        {
            snapping_edge(m_topcellinfos[i], query_box, trans, ld, p, micron_box, snap_point, snap_edge, distance, find_any, mode);
        }
        return ;
    }

    update_time();
    OasisCellInfo& cellinfo = get_cell_info(infoindex);
    OasisCell& cell = get_cell(cellinfo.get_cell_index());
    SnapEdgeVisitor visitor(ld, p, micron_box, get_dbu(), mode);
    visitor.set_layout(this);
    visitor.set_top_query_box(query_box.intersected(cell.get_bbox()));
    visitor.set_top_trans(trans); // empty trans
    cell.accept_visitor(visitor);
    if(visitor.find_any_exact())
    {
        double v_dis = visitor.get_closest_distance_exact();
        if(v_dis < *distance)
        {
            *distance = v_dis;
            *snap_point = visitor.get_closest_point_exact();
            *snap_edge = visitor.get_closest_edge_exact();
            *find_any = true;
        }
    }
    else if(visitor.find_any())
    {
        double v_dis = visitor.get_closest_distance();
        if(v_dis < *distance)
        {
            *distance = v_dis;
            *snap_point = visitor.get_closest_point();
            *snap_edge = visitor.get_closest_edge();
            *find_any = true;
        }

    }
}


void OasisLayout::print_context(Box query_box, OasisTrans trans)
{
    for(uint32 i = 0; i < m_topcellinfos.size(); ++i)
    {
        update_time();
        OasisCellInfo& cellinfo = get_cell_info(m_topcellinfos[i]);
        OasisCell& cell = get_cell(cellinfo.get_cell_index());
        PrintVisitor visitor;
        visitor.set_layout(this);
        visitor.set_top_query_box(query_box.intersected(cell.get_bbox()));
        visitor.set_top_trans(trans);
        cell.accept_visitor(visitor);
    }
}

std::string OasisLayout::print() const
{
    ostringstream os;
    os << "layout information" << endl
        << "#cellinfo " << m_cellinfos.size() << endl
        << "#cell " << m_cells.size() << endl
        << "#defined by cellname " << m_defined_cells_by_name.size() << endl
        << "#defined by cellref " << m_defined_cells_by_ref.size() << endl
        << "#cellname2index " << m_cellname2index.size() << endl
        << "#cellref2index " << m_cellref2index.size() << endl
        << "#cellnames " << m_cellnames.size() << endl
        << "#textstrings " << m_textstrings.size() << endl
        << "#textref2index " << m_textref2index.size() << endl
        << "#textstring2index " << m_textstring2index.size() << endl
        << "#propnames " << m_propnames.size() << endl
        << "#propnameref2index " << m_propnameref2index.size() << endl
        << "#propname2index " << m_propname2index.size() << endl
        << "#propstrings " << m_propstrings.size() << endl
        << "#propstringref2index " << m_propstringref2index.size() << endl
        << "#propstring2index " << m_propstring2index.size() << endl
        << "#layernames " << m_layernames.size() << endl
        << "dbu " << m_dbu << endl;

    return os.str();
}

void OasisLayout::check_file_version()
{
    FileProperty fileprop;

    OasisPropertyBuilder builder;
    builder.decode_file_properties(*this, fileprop, m_file_properties);
    fileprop.get_origin_file_sha1(m_sourcefile_sha1);
    m_bounding_boxes_available = fileprop.m_bounding_boxes_available;
    if (m_major_version != fileprop.m_major_version || m_minor_version != fileprop.m_minor_version)
    {
        throw OasisException("check file version failed");
    }
}

void OasisLayout::set_origin_file_sha1(const uint32 sourcefile_sha1_[])
{
   for (uint32 i = 0; i < 5; ++i) m_sourcefile_sha1[i] = sourcefile_sha1_[i];
}

bool OasisLayout::check_origin_file_sha1(const uint32 sourcefile_sha1_[])
{
   for (uint32 i = 0; i < 5; ++i)
      logger::info <<  "source sha1: " << m_sourcefile_sha1[i] << "; compare with sha1: " << sourcefile_sha1_[i];
   for (uint32 i = 0; i < 5; ++i)
   {
      if (sourcefile_sha1_[i] != m_sourcefile_sha1[i])
         return false;
   }
   return true;
}

void OasisLayout::get_file_properties(std::vector<OasisProperty>& properties)
{
    OasisPropertyBuilder builder;
    FileProperty fileprop;
    fileprop.m_major_version = m_major_version;
    fileprop.m_minor_version = m_minor_version;
    fileprop.set_origin_file_sha1(m_sourcefile_sha1);
    fileprop.m_bounding_boxes_available = 2;
    
    builder.encode_file_properties(*this, fileprop, properties);
}

void OasisLayout::compute_bbox()
{
    OAS_INFO << "begin layout compute bbox" << OAS_ENDL;
    for(uint32 i = 0; i < m_topcellinfos.size(); ++i)
    {
        int64 index = m_topcellinfos[i];
        OasisCellInfo& cellinfo = get_cell_info(index);
        OasisCell& cell = get_cell(cellinfo.get_cell_index());
        cell.compute_bbox(*this);
    }
    OAS_INFO << "end layout compute bbox" << OAS_ENDL;
}

void OasisLayout::update_layers()
{
    OAS_INFO << "begin layout update layers" << OAS_ENDL;
    for (uint32 i = 0; i < m_topcellinfos.size(); ++i)
    {
        int64 index = m_topcellinfos[i];
        OasisCellInfo &cellinfo = get_cell_info(index);
        OasisCell &cell = get_cell(cellinfo.get_cell_index());
        cell.update_layers(*this);
    }
    OAS_INFO << "end layout update layers" << OAS_ENDL;
}

void OasisLayout::compute_layer_bbox(const std::set<LDType> &layers, std::map<int64, std::map<LDType, Box> >& result)
{
    OAS_INFO << "begin compute layer bbox " << OAS_ENDL;
    for (uint32 i = 0; i < m_topcellinfos.size(); ++i)
    {
        int64 index = m_topcellinfos[i];
        OasisCellInfo &cellinfo = get_cell_info(index);
        OasisCell &cell = get_cell(cellinfo.get_cell_index());
        cell.compute_layer_bbox(*this, layers, result);
    }
    OAS_INFO << "end compute layer bbox " << OAS_ENDL;
}

void OasisLayout::update_time()
{
    m_time = (int64)time(NULL);
}

int64 OasisLayout::add_text_string(const std::string& name)
{
    if (m_textstring2index.find(name) == m_textstring2index.end())
    {
        int64 index = m_textstrings.size();
        OasisTextString textstring;
        textstring.set_name(OasisString(name));
        textstring.set_index(index);
        textstring.set_refnum(index);
        m_textstrings.push_back(textstring);
        m_textstring2index[name] = index;
        m_textref2index[index] = index;
        return index;
    }
    else
    {
        return m_textstring2index[name];
    }
}

int64 OasisLayout::add_prop_name(const std::string& name)
{
    if(m_output_propname2index.find(name) == m_output_propname2index.end())
    {
        int64 index = m_output_propnames.size();
        OasisPropName propname;
        propname.set_name(OasisString(name));
        propname.set_index(index);
        propname.set_refnum(index);
        m_output_propnames.push_back(propname);
        m_output_propname2index[name] = index;
        return index;
    }
    else
    {
        return m_output_propname2index[name];
    }
}

int64 OasisLayout::add_prop_string(const std::string& name)
{
    if (m_output_propstring2index.find(name) == m_output_propstring2index.end())
    {
        int64 index = m_output_propstrings.size();
        OasisPropString propstring;
        propstring.set_name(OasisString(name));
        propstring.set_index(index);
        propstring.set_refnum(index);
        m_output_propstrings.push_back(propstring);
        m_output_propstring2index[name] = index;
        return index;
    }
    else
    {
        return m_output_propstring2index[name];
    }
}

const OasisTextString& OasisLayout::get_text_string(int64 refnum)
{
    if (m_textref2index.find(refnum) == m_textref2index.end())
    {
        OAS_ERR << "textstring undefined by ref " << refnum << OAS_ENDL;
        throw OasisException("undefined textstring");
    }
    return m_textstrings[m_textref2index[refnum]];
}

const OasisPropName& OasisLayout::get_prop_name(int64 refnum)
{
    if (m_propnameref2index.find(refnum) == m_propnameref2index.end())
    {
        OAS_ERR << "propname undefined by ref " << refnum << OAS_ENDL;
        throw OasisException("undefined propname");
    }
    return m_propnames[m_propnameref2index[refnum]];
}

const OasisPropString& OasisLayout::get_prop_string(int64 refnum)
{
    if (m_propstringref2index.find(refnum) == m_propstringref2index.end())
    {
        OAS_ERR << "propstring undefined by ref " << refnum << OAS_ENDL;
        throw OasisException("undefined propstring");
    }
    return m_propstrings[m_propstringref2index[refnum]];
}

void OasisLayout::array_layout(const std::string& top_cell, Point origin, uint64 rows, uint64 cols, Point row_vector, Point col_vector)
{
    int64 cellindex = create_cell();
    int64 infoindex = create_cell_info(-1, top_cell);
    OasisCell& cell = get_cell(cellindex);
    OasisCellInfo& info = get_cell_info(infoindex);
    cell.set_infoindex(infoindex);
    info.set_cell_index(cellindex);

    cell.set_stick(true);

    for (size_t i = 0; i < m_topcellinfos.size(); ++i)
    {
        int64 sub_infoindex = m_topcellinfos[i];
        OasisCellInfo &sub_info = get_cell_info(sub_infoindex);
        sub_info.set_referenced();

        PlacementElement *p = new PlacementElement;
        OasisTrans offset;
        offset.shift(origin);
        OasisRepetition repetition;
        OasisRepetitionBuilder builder;
        if (rows == 1 && cols == 1)
        {
            p->create(sub_infoindex, offset);
        }
        else
        {
            if (rows == 1)
            {
                builder.make_diagonal_1d(repetition, cols, OasisDelta(col_vector.x(), col_vector.y()));
            }
            else if (cols == 1)
            {
                builder.make_diagonal_1d(repetition, rows, OasisDelta(row_vector.x(), row_vector.y()));
            }
            else
            {
                builder.make_diagonal_2d(repetition, cols, OasisDelta(col_vector.x(), col_vector.y()), rows, OasisDelta(row_vector.x(), row_vector.y()));
            }
            p->create(sub_infoindex, offset, &repetition);
        }
        cell.add_instance(p);
    }
    cell.compute_bbox(*this);
    cell.update_layers(*this);

    collect_top_cells();
}

void OasisLayout::export_file(OasisWriter& export_writer)
{
    export_writer.write_magic();
    write_start(export_writer);

    for (size_t i = 0; i < m_cellinfos.size(); ++i)
    {
        int64 info_index = i;
        OasisCellInfo &cell_info = get_cell_info(info_index);
        OasisCell &cell = get_cell(cell_info.get_cell_index());
        cell.load(*this);
        cell.write(export_writer);
        cell.unload(*this);
    }

    write_tables(export_writer);
    write_end(export_writer);
}
