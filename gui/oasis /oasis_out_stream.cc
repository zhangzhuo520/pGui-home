#include "oasis_out_stream.h"
#include "oasis_layout.h"

using namespace oasis;

OasisOutStream::OasisOutStream()
{
   m_4x4_option = 0;
   m_rectangle_count = 0;
}

OasisOutStream::~OasisOutStream()
{
    delete m_layout;
}

void OasisOutStream::open_stream(std::string filename)
{
    m_layout = new OasisLayout(filename);
    m_layout->open_file(true);
}

void OasisOutStream::init_stream(float64 dbu, const std::string& topcell)
{
    m_layout->writer().write_magic();
    m_layout->set_dbu(dbu);
    m_layout->write_start(m_layout->writer());

    uint64 cellindex = m_layout->create_cell();
    m_topcell_info_index = m_layout->create_cell_info(cellindex, topcell);
    OasisCellInfo& cellinfo = m_layout->get_cell_info(m_topcell_info_index);
    cellinfo.set_cell_index(cellindex);
    OasisCell& cell = m_layout->get_cell(cellindex);
    cell.set_infoindex(m_topcell_info_index);
}

void OasisOutStream::close_stream()
{
    make_top_tree();

    m_layout->collect_top_cells();
    m_layout->unload();
    
    m_layout->flush_writer(m_layout->writer());

    m_layout->write_tables(m_layout->writer());
    m_layout->write_end(m_layout->writer());
    m_rectangle_count = 0;
}

float64 OasisOutStream::dbu()
{
    return m_layout->get_dbu();
}

void OasisOutStream::compress_geometry(bool enable)
{
    m_layout->compress_geometry(enable);
}

uint64 OasisOutStream::open_cell(const std::string& cellname)
{
    uint64 cellindex = m_layout->create_cell();
    int64 refnum = cellindex;
    int64 info_index = m_layout->create_cell_info(refnum, cellname);
    OasisCellInfo& cellinfo = m_layout->get_cell_info(info_index);
    cellinfo.set_cell_index(cellindex);
    cellinfo.set_referenced();
    OasisCell& cell = m_layout->get_cell(cellindex);
    cell.set_infoindex(info_index);
    return refnum;
}

OasisCell& OasisOutStream::top_cell()
{
    OasisCellInfo& cellinfo = m_layout->get_cell_info(m_topcell_info_index);
    OasisCell& cell = m_layout->get_cell(cellinfo.get_cell_index());
    return cell;
}

void OasisOutStream::close_cell(uint64 cellindex)
{
    OasisCell &cell = get_cell(cellindex);
    cell.compute_bbox(*m_layout);
    cell.update_layers(*m_layout);
    cell.unload(*m_layout);
}

OasisCell& OasisOutStream::new_cell(const std::string& cellname, uint64* infoidx_, uint64* cellidx_)
{
    int64 cellindex = m_layout->create_cell();
    int64 infoindex = m_layout->create_cell_info(cellindex, cellname);
    OasisCellInfo& cellinfo = m_layout->get_cell_info(infoindex);
    cellinfo.set_cell_index(cellindex);
    cellinfo.set_referenced();
    OasisCell& cell = m_layout->get_cell(cellindex);
    cell.set_infoindex(infoindex);
    
    if (infoidx_ != NULL) *infoidx_ = infoindex;
    if (cellidx_ != NULL) *cellidx_ = cellindex;
    return cell;
}

OasisCell& OasisOutStream::get_cell(uint64 cellindex_)
{
    return m_layout->get_cell(cellindex_);
}

uint64 OasisOutStream::get_cell_index(uint64 infoindex_)
{
    OasisCellInfo& cellinfo = m_layout->get_cell_info(infoindex_);
    return cellinfo.get_cell_index();
}

void OasisOutStream::unload_cell(uint64 cellindex_)
{
    OasisCell& cell = m_layout->get_cell(cellindex_);
    cell.unload(*m_layout);
}

void OasisOutStream::add_placement(uint64 cellindex, uint64 refnum, const OasisTrans& trans)
{
    OasisCell &cell = get_cell(cellindex);
    PlacementElement* p = new PlacementElement;
    p->create(refnum, trans);
    cell.add_instance(p);
}

void OasisOutStream::add_rectangle(uint64 cellindex, const Box& box, int layer, int datatype)
{
    OasisCell &cell = get_cell(cellindex);
    RectangleElement* e = new RectangleElement;
    e->create(box, layer, datatype);
    cell.add_geometry(e);
}

void OasisOutStream::add_polygon(uint64 cellindex, const std::vector<Point>& points, int layer, int datatype)
{
    //check rectangle:
    if (points.size() == 4)
    {
        std::set<int64> x, y;
        for (uint8 i = 0; i < 4; ++i)
        {
            x.insert(points[i].x());
            y.insert(points[i].y());
        }
        if (x.size() == 2 && y.size() == 2)
        {
            add_rectangle(cellindex, Box(*(x.begin()), *(y.begin()), *(x.rbegin()), *(y.rbegin())), layer, datatype);
            ++ m_rectangle_count;
            return;
        }
    }

    OasisCell &cell = get_cell(cellindex);
    PolygonElement* e = new PolygonElement;
    e->create(points, layer, datatype);
    cell.add_geometry(e);
}

void OasisOutStream::add_polygon(uint64 cellindex, const Polygon& polygon, int layer, int datatype)
{
    add_polygon(cellindex, polygon.get_points(), layer, datatype);
}

void OasisOutStream::add_text(uint64 cellindex, const Point& point, const std::string& text, int layer, int datatype)
{
   int64 index = m_layout->add_text_string(text);

   OasisTextString textstring;
   textstring.set_index(index);
   textstring.set_refnum(index);
   OasisCell &cell = get_cell(cellindex);
   TextElement* t = new TextElement;
   t->create(point.x(), point.y(), textstring, layer, datatype, NULL);
   cell.add_text(t);
}

void OasisOutStream::add_top_placement(uint64 refnum, const OasisTrans &trans)
{
    PlacementElement* p = new PlacementElement;
    p->create(refnum, trans);
    OasisCell& cell = top_cell();
    cell.add_instance(p);
}

void OasisOutStream::add_top_rectangle(const Box& box, int layer, int datatype)
{
    OasisCell& cell = top_cell();
    RectangleElement* e = new RectangleElement;
    e->create(box, layer, datatype);
    cell.add_geometry(e);
}

void OasisOutStream::add_top_polygon(const std::vector<Point>& points, int layer, int datatype)
{
    OasisCell& cell = top_cell();
    PolygonElement* e = new PolygonElement;
    e->create(points, layer, datatype);
    cell.add_geometry(e);
}

void OasisOutStream::add_top_polygon(const Polygon& polygon, int layer, int datatype)
{
    add_top_polygon(polygon.get_points(), layer, datatype);
}

void OasisOutStream::add_top_text(const Point& point, const std::string& text, int layer, int datatype)
{
   int64 index = m_layout->add_text_string(text);

   OasisTextString textstring;
   textstring.set_index(index);
   textstring.set_refnum(index);
   OasisCell& cell = top_cell();
   TextElement* t = new TextElement;
   t->create(point.x(), point.y(), textstring, layer, datatype, NULL);
   cell.add_text(t);
}


void OasisOutStream::add_layer_name(const std::string& name, int layer, int datatype)
{
    m_layout->add_layer_name(layer, datatype, name);
}

void OasisOutStream::add_raw_data(uint64 cellindex, const Box& box, const std::vector<std::pair<int, int> >& layers, const std::vector<std::string>& layer_names, const uint8* data, int32 data_size)
{
    OasisCell& cell = get_cell(cellindex);
    cell.assign_raw_data(*m_layout, box, layers, data, data_size);

    for (size_t i = 0; i < layer_names.size(); ++i)
    {
        if (!layer_names[i].empty())
        {
            auto layer = layers[i];
            m_layout->add_layer_name(layer.first, layer.second, layer_names[i]);
        }
    }
}

void OasisOutStream::make_top_tree()
{
    std::string name = "cellholder";
    OasisCell& cell = top_cell();
    
    if (m_4x4_option >= 0) 
    {
        int level = 0;
        if (m_4x4_option == 0)
        {
            cell.make_top_tree_v2(*m_layout, name, level);
        }
        else
        {
            cell.make_top_tree(*m_layout, name, level);
        }
      
        cell.separate_geometry(*m_layout);
    }
    cell.compute_bbox(*m_layout);
    cell.update_layers(*m_layout);
}

RawCellStream::RawCellStream()
{
    m_writer.get_modal_variables().reset();
    m_writer.enter_cblock();
}

RawCellStream::~RawCellStream()
{
}

void RawCellStream::done()
{
    m_writer.leave_cblock();
}

void RawCellStream::add_placement(uint64 refnum, const OasisTrans& trans)
{
    PlacementElement p;
    p.create(refnum, trans);
    p.write(m_writer);
}

void RawCellStream::add_rectangle(const Box& box, int layer, int datatype)
{
    RectangleElement e;
    e.create(box, layer, datatype);
    e.write(m_writer);
}

void RawCellStream::add_polygon(const std::vector<Point>& points, int layer, int datatype)
{
    //check rectangle:
    if (points.size() == 4)
    {
        std::set<int64> x, y;
        for (uint8 i = 0; i < 4; ++i)
        {
            x.insert(points[i].x());
            y.insert(points[i].y());
        }
        if (x.size() == 2 && y.size() == 2)
        {
            add_rectangle(Box(*(x.begin()), *(y.begin()), *(x.rbegin()), *(y.rbegin())), layer, datatype);
            return;
        }
    }

    PolygonElement e;
    e.create(points, layer, datatype);
    e.write(m_writer);
}

void RawCellStream::add_polygon(const Polygon& polygon, int layer, int datatype)
{
    add_polygon(polygon.get_points(), layer, datatype);
}

const uint8* RawCellStream::get_data() const
{
    return m_writer.get_data();
}

const uint32 RawCellStream::get_size() const
{
    return m_writer.get_size();
}
