#ifndef OASIS_LAYOUT_H
#define OASIS_LAYOUT_H
#include "oasis_types.h"
#include "oasis_writer.h"
#include "oasis_cell.h"
#include <map>
#include <vector>
#include <set>
#include <utility>
#include <unordered_map>

namespace render
{
    class Bitmap;
}

namespace oasis
{

class OasisLayout
{
    friend class ParserBase;
    friend class OasisParser;
    friend class GDSParser;
    friend class OasisOutStream;
public:
    OasisLayout(const std::string& filename);
    ~OasisLayout();

    void add_property(const OasisProperty& property) { m_file_properties.push_back(property); }
    void get_file_properties(std::vector<OasisProperty>& properties);
    uint64 major_version() { return m_major_version; }
    uint64 minor_version() { return m_minor_version; }
    void set_origin_file_sha1(const uint32 sourcefile_sha1_[]);
    bool check_origin_file_sha1(const uint32 sourcefile_sha1_[]);
       
    std::string get_file_name() const { return m_filename; }
    void set_dbu(float64 dbu) { m_dbu = dbu; }
    float64 get_dbu() const { return m_dbu; }
    Box get_bbox();
    Box cell_bbox(int64 index);
    std::string cell_name(int64 index);
   
    void write_start(OasisWriter& writer);
    void write_end(OasisWriter& writer);
    void write_tables(OasisWriter& writer);
    void flush_writer(OasisWriter& writer);

    void array_layout(const std::string& top_cell, Point origin, uint64 rows, uint64 cols, Point row_vector, Point col_vector);
    void export_file(OasisWriter& writer);

    int64 create_cell_info(int64 refnum, const std::string& cellname);
    uint64 create_cell(bool import = false);
    OasisCell& get_cell(uint64 index);
    const OasisCell& get_cell(uint64 index) const;
    OasisCellInfo& get_cell_info(int64 index);
    const OasisCellInfo& get_cell_info(int64 index) const;
    uint32 get_cell_count() const { return m_cells.size(); }
    void add_layer_name(int layer, int datatype, const std::string& name);
    void get_layers(std::set<std::pair<int, int> >& layers) const;
    void insert_layer(int32 layer, int32 datatype) { m_layers.insert(std::make_pair(layer, datatype)); }
    void update_layers(const std::set<std::pair<int32, int32> >& layers);
    std::string get_layer_name(int layer, int datatype) const;
    std::vector<int64> top_cell_infos() const { return m_topcellinfos; }
    int64 info_index_by_name(const std::string& name);
    const std::vector<int64>& top_down_infos() const { return m_top_down_infos; }

    int64 add_text_string(const std::string& name);
    int64 add_prop_name(const std::string& name);
    int64 add_prop_string(const std::string& name);
    const OasisTextString& get_text_string(int64 refnum);
    const OasisPropName& get_prop_name(int64 refnum);
    const OasisPropString& get_prop_string(int64 refnum);

    int64 repetition_dimension_limit() const { return (int64)rint(50 / m_dbu); }
    static uint64 geometry_count_limit() { return 100000; }
    static uint64 large_geometry_count_limit() { return 1000; }
    static uint64 instance_count_limit() { return 100; }
    int64 max_geometry_dimension() const { return (int64)rint(100 / m_dbu); }
    int64 max_geometry_cell_dimension() const { return (int64)rint(50 / m_dbu); }

    void compute_bbox();
    void update_layers();
    void compute_layer_bbox(const std::set<LDType> &layers, std::map<int64, std::map<LDType, Box> >& result);

    void resolve_alias();

    void get_polygons(int64 infoindex, Box query_box, OasisTrans trans, const LDType& ld, std::vector<Polygon>& results);

    void get_polygons_with_attachments(
        int64 infoindex,
        Box query_box,
        OasisTrans trans,
        const LDType &ld,
        std::vector<Polygon> *results,
        std::vector<int32> *attach_keys);

    void select_polygons(int64 top_index, int64 select_index, const LDType& ld, std::vector<Polygon>& results);

    int32 count_mark(int64 top_index, const std::string& subname, std::map<std::string, std::string>& parent_to_subcell, std::map<std::string, int>& corner_to_count);

    void draw_bitmap(
        int64 infoindex,
        Box query_box,
        OasisTrans trans,
        const LDType &ld,
        uint32 width,
        uint32 height,
        float64 resolution,
        render::Bitmap* contour,
        render::Bitmap* fill,
        render::Bitmap* vertex);

    void snapping_edge(
        int64 infoindex,
        Box query_box,
        OasisTrans trans,
        const LDType & ld,
        const PointF& p,
        const BoxF& exact_box,
        PointF* snap_point,
        EdgeF* snap_edge,
        double* distance,
        bool* find_any,
        int mode);

    void print_context(Box query_box, OasisTrans trans);

    void build_hierarchy();
    void print_hierarchy();
    void get_all_leaves(std::set<int32>& leaves);
    void get_all_instances(int64 infoindex, const std::set<int32>& leaves, std::map<int32, std::vector<OasisTrans> >& instances);
    void load_all_cell();
    void unload(bool force = false, int expire = -1);
    uint64 count_objects();
    OasisWriter& writer() { return m_writer; }

    std::string print() const;

    FILE* load_fp() { return m_loadfp; }

    void update_time();
    int64 get_time() { return m_time; }

    void multiple_thread_write(bool enable) { m_multiple_thread_write = enable; }
    bool multiple_thread_write() const { return m_multiple_thread_write; }

    void compress_geometry(bool enable) { m_compress_geometry = enable; }
    bool compress_geometry() const { return m_compress_geometry; }
private:
    void open_file(bool w = false);
    void cleanup();
    void collect_top_cells();
    void check_file_version();
    bool cellname_strictmode() const { return m_cellname_flag; }
    bool layername_strictmode() const { return m_layername_flag; }
    bool textstring_strictmode() const { return m_textstring_flag; }
    bool propname_strictmode() const { return m_propname_flag; }
    bool propstring_strictmode() const { return m_propstring_flag; }
    bool xname_strictmode() const { return m_xname_flag; }

    void collect_children(std::pair<uint32, uint32> range);
private:
    std::map<int64, std::set<int64> > m_info2children;
    std::map<int64, std::set<int64> > m_info2parents;

    sys::Mutex m_hierarchy_locker;
    std::map<int64, std::set<int64> > m_children_map;
    std::map<int64, std::set<int64> > m_parents_map;

    mutable sys::Mutex m_create_locker;
    std::unordered_map<int64, OasisCellInfo> m_cellinfos; //key range [0, size)
    std::unordered_map<uint64, OasisCell*> m_cells; //key range [0, size)
    std::vector<int64> m_top_down_infos;

    std::set<std::string> m_defined_cells_by_name;
    std::set<uint64> m_defined_cells_by_ref;
    std::map<std::string, int64> m_cellname2index;
    std::map<uint64, int64> m_cellref2index;
    std::map<uint64, OasisString> m_cellnames;

    std::vector<OasisTextString> m_textstrings;
    std::map<uint64, int64> m_textref2index;
    std::map<std::string, int64> m_textstring2index;

    std::vector<OasisPropName> m_propnames;
    std::map<uint64, int64> m_propnameref2index;
    std::map<std::string, int64> m_propname2index;

    std::vector<OasisPropName> m_output_propnames;
    std::map<std::string, int64> m_output_propname2index;

    std::vector<OasisPropString> m_propstrings;
    std::map<uint64, int64> m_propstringref2index;
    std::map<std::string, int64> m_propstring2index;

    std::vector<OasisPropString> m_output_propstrings;
    std::map<std::string, int64> m_output_propstring2index;

    std::vector<OasisLayerName> m_layernames;
    std::vector<int64> m_topcellinfos;
    sys::Mutex m_layers_locker;
    std::set<std::pair<int32, int32> > m_layers;
    float64 m_dbu;

    std::vector<OasisProperty> m_file_properties;
    
    uint64 m_major_version;
    uint64 m_minor_version;
    uint32 m_sourcefile_sha1[5];
    uint32 m_bounding_boxes_available;

    bool m_cellname_flag;
    bool m_textstring_flag;
    bool m_propname_flag;
    bool m_propstring_flag;
    bool m_layername_flag;
    bool m_xname_flag;
    uint64 m_cellname_offset;
    uint64 m_textstring_offset;
    uint64 m_propname_offset;
    uint64 m_propstring_offset;
    uint64 m_layername_offset;
    uint64 m_xname_offset;

    std::string m_filename;
    FILE* m_loadfp;
    FILE* m_writefp;
    OasisWriter m_writer;
    bool m_multiple_thread_write;
    bool m_compress_geometry;

    int64 m_time;
};

}

#endif
