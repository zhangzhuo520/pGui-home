#ifndef DRAWBITMAPVISITOR_H
#define DRAWBITMAPVISITOR_H
#include "cell_visitor.h"
#include "render_bitmap.h"
#include "render_cell_cache.h"
#include "render_hash.h"
#include <map>
#include <unordered_map>
#include <set>

namespace render
{
    class RenderThread;
}

namespace oasis
{

class DrawBitmapVisitor : public CellVisitor
{
public:
    typedef render::CellCacheManager::cell_cache_type cell_cache_type;
    typedef typename cell_cache_type::iterator cell_cache_iter;
    DrawBitmapVisitor(
            unsigned long levels,
            int block_index,
            OasisTrans dbu_trans,
//            std::map<LDType, int>& lds,
            std::unordered_map<LDType, int, render::hash_LDType, render::cmp_LDType>& lds,
            std::vector<render::Bitmap*>&bitmaps,
            render::CellCacheManager* cache,
            render::RenderThread* thread);

    virtual void visit_instances(const std::vector<OasisElement*>& elements);
    virtual void visit_geometries(const std::vector<OasisElement*>& elements);
    virtual void visit_texts(const std::vector<OasisElement*>& elements) { }

    void print_hits(int);
private:
    unsigned long m_levels;
    int m_block_index;
    OasisTrans m_dbu_trans;
//    std::map<LDType, int>& m_ld_map;
    std::unordered_map<LDType, int, render::hash_LDType, render::cmp_LDType>& m_ld_map;
    std::vector<render::Bitmap*>& m_bitmaps;
    render::CellCacheManager* m_cache;
    render::RenderThread* m_thread;
    const int planes_per_layer;

    void normal_render(const std::vector<OasisElement*> &elements, OasisTrans& tran, Box& qbox, int levels);
    void initialize_cache(const std::vector<OasisElement*> & elements, OasisTrans& trans, Box& box, render::CellCacheStatus& status, int levels);
    void cp_bitmap(std::map<LDType, render::BitmapInfo>& bitmap_groups, int offset, int x, int y);


    cell_cache_iter find_cell(int cell_index, OasisTrans& trans);
    void fast_draw_cell(render::CellCacheStatus& status, OasisTrans& trans);

    bool draw_polygon_array(oasis::Box& box, oasis::Box& box_with_repetition, oasis::Box&qbox, oasis::OasisTrans& trans,
                            std::vector<Point>& disps, render::Bitmap* contour_bitmap, render::Bitmap* fill_bitmap);

    void draw_cell_box(oasis::Box& box, oasis::OasisTrans& trans, render::Bitmap* contour_bitmap, render::Bitmap* fill_bitmap);

    void draw_polygon(oasis::Polygon& polygon, oasis::Box& bbox, oasis::OasisTrans& trans, render::Bitmap* contour_bitmap, render::Bitmap* fill_bitmap);

    int m_fast_draw_array_times;
    int m_normal_render_times;
    int m_fast_draw_cell_times;
    int m_single_draw_polygon_times;
    int m_single_draw_box_times;
    int m_repetition_times;
    std::set<std::string> m_fast_draw_cell_name;
    std::set<std::string> m_normal_draw_cell_name;
    int m_draw_dot_times;
    int m_draw_out_of_box_times;
    int m_draw_box_times;
    int m_elements;
    bool m_draw_instances;
    std::vector<std::pair<BoxF, Point> >m_out_of_box;
    std::vector<Box> m_draw_cell_box;
};

}
#endif
