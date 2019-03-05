#ifndef OASIS_CELL_H
#define OASIS_CELL_H
#include "oasis_types.h"
#include "thread_base.h"
#include <typeinfo>
#include <vector>
#include <set>
#include <utility>


namespace oasis
{

class OasisLayout;
class OasisWriter;
class CellVisitor;

class OasisReferenceString
{
public:
    OasisReferenceString() : m_index(-1), m_alias(-1), m_refed(false), m_named(false) {}

    bool is_referenced() const { return m_refed; }
    bool is_named() const { return m_named; }
    void set_index(int64 index) { m_index = index; }
    void set_alias(int64 index) { m_alias = index; }
    void set_refnum(uint64 refnum) { m_refnum = refnum; m_refed = true; }
    void set_name(const OasisString& str) { m_string = str; m_named = true; }
    int64 get_index() const { return m_index; }
    int64 get_alias() const { return m_alias; }
    uint64 get_refnum() const { return m_refnum; }
    const OasisString& get_name() const { return m_string; }

    bool operator==(const OasisReferenceString& rhs) const
    {
        return m_refed == rhs.m_refed &&
                m_named == rhs.m_named &&
                (m_refed ? (m_refnum == rhs.m_refnum) : true) &&
                (m_named ? (m_string == rhs.m_string) : true);
    }
    bool operator!=(const OasisReferenceString& rhs) const
    {
        return !operator==(rhs);
    }
protected:
    int64 m_index;
    int64 m_alias;
    uint64 m_refnum;
    OasisString m_string;
    bool m_refed;
    bool m_named;
};

class OasisTextString : public OasisReferenceString
{
public:
    void write(OasisWriter& writer);
};

class OasisPropName : public OasisReferenceString
{
    friend class OasisPropertyBuilder;
public:
    void write(OasisWriter& writer);
};

class OasisPropString : public OasisReferenceString
{
    friend class OasisPropertyBuilder;
public:
    void write(OasisWriter& writer);
};

class OasisLayerName
{
public:
    OasisLayerName() : m_is_text_layer(false) {}
    OasisLayerName(const OasisString& name, const OasisInterval& layer_interval, const OasisInterval& datatype_interval, bool is_textlayer) :
        m_is_text_layer(is_textlayer), m_layer_name(name), m_layer_interval(layer_interval), m_datatype_interval(datatype_interval) {}
    void write(OasisWriter& writer);

    bool contains(uint32 layer, uint32 datatype) const
    {
        return m_layer_interval.contains(layer) && m_datatype_interval.contains(datatype);
    }

    std::string get_name() const
    {
        return m_layer_name.value();
    }

    bool operator==(const OasisLayerName& rh) const
    {
        return m_is_text_layer == rh.m_is_text_layer &&
            m_layer_name == rh.m_layer_name &&
            m_layer_interval == rh.m_layer_interval &&
            m_datatype_interval == rh.m_datatype_interval;
    }

    bool operator!=(const OasisLayerName& rh) const
    {
        return !operator==(rh);
    }
private:
    bool m_is_text_layer;
    OasisString m_layer_name;
    OasisInterval m_layer_interval;
    OasisInterval m_datatype_interval;
};

class OasisProperty
{
    friend class OasisPropertyBuilder;
public:
    OasisProperty() : m_is_standard(false) {}
    void create(bool is_std, const OasisPropName& propname, const std::vector<OasisPropValue>& propvalues);
    void write(OasisWriter& writer);
private:
    bool m_is_standard;
    OasisPropName m_prop_name;
    std::vector<OasisPropValue> m_prop_values;
};

class OasisElement
{
public:
    OasisElement() : m_repetition(NULL) {}
    virtual ~OasisElement() { delete m_repetition; }

    virtual OasisElement* clone() { oas_assert(false); return NULL; }
    virtual void polygon(Polygon& p) { oas_assert(false); }
    virtual void bbox(Box& box) { oas_assert(false); }
    virtual void offset(Point& p) { oas_assert(false); }
    virtual void write(OasisWriter& writer) = 0;
    virtual bool compare(OasisElement* rh) { return this < rh; }
    virtual bool equal(OasisElement* rh) { return this == rh; }
    virtual bool valid() const { return true; }

    bool has_repetition() const { return m_repetition != NULL; }
    void write_repetition(OasisWriter& writer);
    void set_repetition(const OasisRepetition& repetition);
    void get_disps(std::vector<Point>& disps) const;
    void get_bbox_with_repetition(Box& box);

    void set_ld(uint64 layer, uint64 datatype) { m_layer = layer; m_datatype = datatype; }

    int32 layer() const { return m_layer; }
    int32 datatype() const { return m_datatype; }
protected:
    void set_base(OasisElement* e) { set_ld(e->layer(), e->datatype()); if(e->m_repetition) set_repetition(*e->m_repetition); }
    OasisRepetition* m_repetition;
    uint32 m_layer;
    uint32 m_datatype;
};

inline bool compare_element(OasisElement* a, OasisElement* b)
{
    if(a->layer() != b->layer())
    {
        return a->layer() < b->layer();
    }
    if(a->datatype() != b->datatype())
    {
        return a->datatype() < b->datatype();
    }
    if(typeid(*a).name() != typeid(*b).name())
    {
        return typeid(*a).name() < typeid(*b).name();
    }

    return a->compare(b);
}

inline bool equal_element(OasisElement* a, OasisElement* b)
{
    if(a->layer() != b->layer())
    {
        return false;
    }
    if(a->datatype() != b->datatype())
    {
        return false;
    }
    if(typeid(*a).name() != typeid(*b).name())
    {
        return false;
    }
    return a->equal(b);
}

class RectangleElement : public OasisElement
{
public:
    void create(int64 x, int64 y, uint64 w, uint64 h, uint64 layer, uint64 datatype, OasisRepetition* repetition = NULL);
    void create(const Box& box, uint64 layer, uint64 datatype, OasisRepetition* repetition = NULL);
    virtual OasisElement* clone();
    virtual void polygon(Polygon& p);
    virtual void bbox(Box& box) { box = m_rect; }
    virtual void offset(Point &p) { p.set_x(m_rect.left()); p.set_y(m_rect.bottom()); }
    virtual void write(OasisWriter& writer);
    virtual bool compare(OasisElement* rh);
    virtual bool equal(OasisElement* rh);
private:
    Box m_rect;
};

class PolygonElement : public OasisElement
{
public:
    PolygonElement(): m_valid(1) {}
    void create(int64 x, int64 y, const OasisPointList& pointlist, uint64 layer, uint64 datatype, OasisRepetition* repetition = NULL);
    void create(const std::vector<Point>& points, uint64 layer, uint64 datatype, OasisRepetition* repetition = NULL);
    void create(int64 x, int64 y, const std::vector<Point>& points, uint64 layer, uint64 datatype, OasisRepetition* repetition = NULL);
    virtual OasisElement* clone();
    virtual void polygon(Polygon& p);
    virtual void bbox(Box& box);
    virtual void offset(Point& p) { p = m_start_point; }
    virtual void write(OasisWriter& writer);
    virtual bool compare(OasisElement* rh);
    virtual bool equal(OasisElement* rh);
    virtual bool valid() const { return m_valid > 0; }
private:
    Point m_start_point;
    OasisPointList m_point_list;
    uint8 m_valid;
};

class PathElement : public OasisElement
{
public:
    PathElement(): m_valid(1) {}
    void create(int64 x, int64 y, int64 startext, int64 endext, uint64 halfwidth, const OasisPointList& pointlist, uint64 layer, uint64 datatype, OasisRepetition* repetition = NULL);
    void create(int64 startext, int64 endext, uint64 halfwidth, const std::vector<Point>& points, uint64 layer, uint64 datatype, OasisRepetition* repetition = NULL);
    void create(int64 x, int64 y, int64 startext, int64 endext, uint64 halfwidth, const std::vector<Point>& points, uint64 layer, uint64 datatype, OasisRepetition* repetition = NULL);
    virtual OasisElement* clone();
    virtual void polygon(Polygon& p);
    virtual void bbox(Box& box);
    virtual void offset(Point& p) { p = m_start_point; }
    virtual void write(OasisWriter& writer);
    virtual bool compare(OasisElement* rh);
    virtual bool equal(OasisElement* rh);
    virtual bool valid() const { return m_valid > 0; }
private:
    Point m_start_point;
    int64 m_start_ext;
    int64 m_end_ext;
    uint64 m_halfwidth;
    OasisPointList m_point_list;
    uint8 m_valid;
};

class TextElement : public OasisElement
{
public:
    void create(int64 x, int64 y, const OasisTextString& textstring, uint64 textlayer, uint64 texttype, OasisRepetition* repetition = NULL);
    virtual OasisElement* clone();
    virtual void bbox(Box& box);
    virtual void offset(Point& p) { p = m_start_point; }
    virtual void write(OasisWriter& writer);
private:
    OasisTextString m_textstring;
    Point m_start_point;
};

class PlacementElement : public OasisElement
{
public:
    void create(int64 infoindex, const OasisTrans& trans, OasisRepetition* repetition = NULL);
    virtual OasisElement* clone();
    virtual void write(OasisWriter& writer);

    void set_info_index(int64 idx) { m_infoindex = idx; }
    int64 get_info_index() const { return m_infoindex; }
    const OasisTrans& get_trans() const { return m_trans; }
    Box get_bbox(OasisLayout& layout) const;
    Box get_bbox_with_repetition(OasisLayout& layout) const;
    Box get_bbox_with_repetition(const Box& box) const;
private:
    int64 m_infoindex;
    OasisTrans m_trans;
};

class OasisCell
{
public:
    OasisCell(bool import = false) : m_stick(false), m_saved(false), m_cached(true),
                                     m_boxready(false), m_geomboxready(false), m_layers_ready(false), m_children_ready(false),
                                     m_import_mode(import), m_use_count(0), m_use_time(0)
    {
        if(import)
        {
            m_saved = true;
            m_cached = false;
        }
    }
    ~OasisCell() { cleanup(true); }
    void write(OasisWriter& writer);
    void clone(OasisCell& dest, OasisLayout& layout, int64 infoindex = -1);

    void add_geometry(OasisElement* e);
    void add_text(OasisElement* e);
    void add_instance(OasisElement* e);

    void partition(OasisLayout& layout);
    void separate_geometry(OasisLayout& layout);
    void partition_geometry(OasisLayout& layout);
    void partition_instance(OasisLayout& layout);
    void compute_bbox(OasisLayout& layout);
    bool try_compute_bbox(OasisLayout& layout);
    void update_layers(OasisLayout& layout);
    bool try_update_layers(OasisLayout& layout);
    void collect_children(OasisLayout& layout);
    void collect_parents(OasisLayout& layout);
    const std::set<int64>& child_infos() const { return m_children_infos; }
    const std::set<int64>& parent_infos() const { return m_parents_infos; }
    void print_children();
    void print_parents();

    void compute_layer_bbox(OasisLayout &layout, const std::set<LDType> &layers, std::map<int64, std::map<LDType, Box> >& result);

    void assign_raw_data(OasisLayout& layout, const Box& bbox, const std::vector<std::pair<int32, int32> >& layers, const uint8* data, int32 data_size);

    void compress();

    int64 get_infoindex() const { return m_infoindex; }
    void set_infoindex(int64 index) { m_infoindex = index; }

    void accept_visitor(CellVisitor& visitor);

    void flush(OasisLayout& layout);
    void load(OasisLayout& layout);
    void cleanup(bool force = false);
    uint64 count_objects();

    void unload(OasisLayout& layout);
    void make_top_tree(OasisLayout& layout, const std::string& cellname, int level);
    void make_top_tree_v2(OasisLayout& layout, const std::string& cellname, int level);

    int64 get_offset() const { return m_fileoffset; }
    void set_offset(int64 offset) { oas_assert(offset > 0); m_fileoffset = offset; }

    int64 get_hint_datasize() const { return m_hint_datasize; }
    void set_hint_datasize(int64 size) { m_hint_datasize = size; }

    const Box& get_bbox() const { oas_assert(m_boxready); return m_bbox; }
    void set_bbox(const Box& bbox) { m_bbox = bbox; m_boxready = true; }

    bool get_stick() const { return m_stick; }
    void set_stick(bool stick) { m_stick = stick; }

    void update_use_time(int64 time) { m_use_time = time; }
    int64 use_time() const { return m_use_time; }

    const std::vector<OasisElement*>& get_instances() const { return m_instances; }
    const std::vector<OasisElement*>& get_geometries() const { return m_geometries; }
    const std::vector<OasisElement*>& get_texts() const { return m_texts; }

private:
    void parse_load(OasisLayout& layout);
    void import_load(OasisLayout& layout);
    bool unused() { return m_use_count <= 0; }
    void increase_use_count() { sys::MutexGuard guard(m_locker); ++m_use_count; }
    void decrease_use_count() { sys::MutexGuard guard(m_locker); --m_use_count; }

private:
    int64 m_infoindex;
    std::vector<OasisElement*> m_geometries;
    std::vector<OasisElement*> m_texts;
    std::vector<OasisElement*> m_instances;

    std::set<int64> m_children_infos;
    std::set<int64> m_parents_infos;

    int64 m_fileoffset;
    int64 m_hint_datasize;
    bool m_stick;
    bool m_saved;
    bool m_cached;
    bool m_boxready;
    bool m_geomboxready;
    bool m_layers_ready;
    bool m_children_ready;
    bool m_import_mode;
    Box m_bbox;
    sys::Mutex m_locker;
    uint16 m_use_count;
    int64 m_use_time;
};

class OasisCellInfo
{
public:
    enum Flag {Defined = 1, Referenced = 2, Named = 4};
    OasisCellInfo() : m_refnum(-1), m_cellindex(-1), m_alias(-1), m_flag(0) {}

    void set_name(const std::string& name) { m_name = name; }
    void set_ref_num(int64 ref) { m_refnum = ref; }
    void set_alias(int64 index) { m_alias = index; }
    void set_cell_index(int64 cellindex) { if(cellindex >= 0) { m_cellindex = cellindex; set_flag(Defined); } }
    void set_named() { set_flag(Named); }
    void set_referenced() { set_flag(Referenced); }
    void set_flag(uint32 f) { m_flag |= f; }

    std::string get_name() const { return m_name; }
    uint32 get_flag() const { return m_flag; }
    int64 get_ref_num() const { return m_refnum; }
    int64 get_alias() const { return m_alias; }
    int64 get_cell_index() const { return m_cellindex; }

    bool is_defined() const { return (m_flag & Defined) != 0; }
    bool is_referenced() const { return (m_flag & Referenced) != 0; }
    bool is_named() const { return (m_flag & Named) != 0; }
    bool has_name() const { return !m_name.empty(); }
    bool has_ref_num() const { return m_refnum >= 0; }

    void add_property(const OasisProperty& property) { m_properties.push_back(property); }
    void apply_properties(OasisLayout& layout);
    void get_properties(OasisLayout& layout, std::vector<OasisProperty>& properties);
    
private:
    std::string m_name;
    int64 m_refnum;
    int64 m_cellindex;
    int64 m_alias;
    uint32 m_flag;
    std::vector<OasisProperty> m_properties;
};



}
#endif
