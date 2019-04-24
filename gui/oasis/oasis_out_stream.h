#ifndef OASIS_OUTSTREAM_H
#define OASIS_OUTSTREAM_H
#include "oasis_writer.h"
#include "oasis_cell.h"
#include <string>
#include <set>
#include <vector>
#include <utility>


namespace oasis
{

class OasisOutStream
{
public:
    OasisOutStream();
    ~OasisOutStream();

    void open_stream(std::string filename);
    void init_stream(float64 dbu, const std::string& topcell);
    void close_stream();

    float64 dbu();
    void set_top_tree_option(int32 op_) {m_4x4_option = op_;}
    void compress_geometry(bool enable);

    uint64 open_cell(const std::string& cellname);
    void close_cell(uint64 cellindex);
    
    OasisCell& new_cell(const std::string& cellname, uint64* infoidx_ = NULL, uint64* cellidx_ = NULL);
    uint64 get_cell_index(uint64 infoindex_);
    OasisCell& get_cell(uint64 cellindex_);
    void unload_cell(uint64 cellindex_);

    void add_placement(uint64 cellindex, uint64 refnum, const OasisTrans& trans);
    void add_rectangle(uint64 cellindex, const Box& box, int layer, int datatype);
    void add_polygon(uint64 cellindex, const std::vector<Point>& points, int layer, int datatype);
    void add_polygon(uint64 cellindex, const Polygon& polygon, int layer, int datatype);
    void add_text(uint64 cellindex, const Point& point, const std::string& text, int layer, int datatype);
    void add_top_placement(uint64 refnum, const OasisTrans& trans);
    void add_top_polygon(const std::vector<Point>& points, int layer, int datatype);
    void add_top_polygon(const Polygon& polygon, int layer, int datatype);
    void add_top_rectangle(const Box& box, int layer, int datatype);
    void add_top_text(const Point& point, const std::string& text, int layer, int datatype);
    int64 count_converted_rectangles() { return m_rectangle_count; }
    void add_layer_name(const std::string& name, int layer, int datatype);
    void add_raw_data(
        uint64 cellindex,
        const Box& bbox,
        const std::vector<std::pair<int, int> >& layers,
        const std::vector<std::string>& layer_names,
        const uint8* data,
        int32 data_size);

private:
    OasisCell& top_cell();
    void make_top_tree();

private:
    OasisLayout* m_layout;
    int64 m_topcell_info_index;
    int32 m_4x4_option;
    int64 m_rectangle_count;
};

class RawCellStream
{
public:
    RawCellStream();
    ~RawCellStream();
    void done();
    void add_placement(uint64 refnum, const OasisTrans& trans);
    void add_rectangle(const Box& box, int layer, int datatype);
    void add_polygon(const std::vector<Point>& points, int layer, int datatype);
    void add_polygon(const Polygon& polygon, int layer, int datatype);

    const uint8* get_data() const;
    const uint32 get_size() const;
private:
    MemoryOasisWriter m_writer;
};

}

#endif
