#include <QDebug>
#include <QString>

#include "draw_bitmap_visitor.h"
#include "oasis_layout.h"
#include "oasis_cell.h"
#include "stdint.h"
#include "render_multithread_manager.h"

#include <sstream>
using namespace oasis;

static bool is_small_cell(const BoxF& box)
{
   float64 tmp = 0.0;
   tmp = std::max(box.width(), box.height());
   return tmp < 2.5;
}


static bool inside(const Box& cell_box, const Box& qbox)
{
    if(cell_box.is_empty() || qbox.is_empty())
    {
        return false;
    }
    else
    {
        return (cell_box.left() >= qbox.left() && cell_box.right() <= qbox.right()&&
                cell_box.bottom() >= qbox.bottom() && cell_box.top() <= qbox.top());
    }
}

static bool enable_simplify_box(Box& b, const OasisTrans& trans)
{
    const double threshold = 2.0;
    double fabs_mag = fabs(trans.mag());
    double width = fabs_mag * b.width();
    double height = fabs_mag * b.height();
    if( width < threshold || height < threshold)
    {
        if(width < threshold)
        {
            int64 center_x = b.left() + b.width() / 2;
            b.set_left(center_x);
            b.set_right(center_x);
        }
        if(height < threshold)
        {
            int64 center_y = b.bottom() + b.height() / 2;
            b.set_bottom(center_y);
            b.set_top(center_y);
        }
        return true;
    }
    else
    {
        return false;
    }
}

static void draw_dot(oasis::PointF& p, render::Bitmap* contour_bitmap, render::Bitmap* fill_bitmap)
{
    float64 x = p.x();
    float64 y = p.y();
    x += 0.5;
    y += 0.5;
    int64 top = contour_bitmap->height();
    int64 right = contour_bitmap->width();
    if(x < 0.0 || x >= right || y < 0.0 || y >= top)
    {
        return ;
    }

    unsigned int int_x = (unsigned int)x;
    unsigned int int_y = (unsigned int)y;
    contour_bitmap->fill(int_y, int_x, int_x + 1);
    fill_bitmap->fill(int_y, int_x, int_x + 1);
}

static void draw_box(oasis::Box& box, oasis::OasisTrans& trans, render::Bitmap* contour_bitmap, render::Bitmap* fill_bitmap)
{
    if(box.is_empty())
    {
       return ;
    }

    BoxF transed_box = box.transedF(trans);

    double threshold = 1.0 / fabs(trans.mag());
    if (box.width() < threshold && box.height() < threshold)
    {
        PointF center(transed_box.left() + transed_box.width() / 2, transed_box.bottom() + transed_box.height() / 2);
        draw_dot(center, contour_bitmap, fill_bitmap);
        return ;
    }

    float64 x_min = transed_box.left();
    float64 x_max = transed_box.right();
    float64 y_min = transed_box.bottom();
    float64 y_max = transed_box.top();

    int64 height = fill_bitmap->height();
    int64 width = fill_bitmap->width();


    x_min += 0.5;
    x_max += 0.5;
    y_min += 0.5;
    y_max += 0.5;

    if(x_max < 0.0 || x_min >= width || y_max < 0.0 || y_min >= height)
    {
        return ;
    }

    unsigned int y1 = (unsigned int) (std::max(0.0, std::min(y_min, double(height - 1))));
    unsigned int y2 = (unsigned int) (std::max(0.0, std::min(y_max, double(height - 1))));
    unsigned int x1 = (unsigned int) (std::max(0.0, std::min(x_min, double(width - 1))));
    unsigned int x2 = (unsigned int) (std::max(0.0, std::min(x_max, double(width - 1))));

    for(unsigned int i = y1; i <= y2; i++)
    {
        fill_bitmap->fill(i, x1, x2 + 1);
    }

    std::vector<render::RenderEdge> ortho_edges;
    ortho_edges.push_back(render::RenderEdge(x_min, y_min, x_max, y_min));
    ortho_edges.push_back(render::RenderEdge(x_max, y_min, x_max, y_max));
    ortho_edges.push_back(render::RenderEdge(x_max, y_max, x_min, y_max));
    ortho_edges.push_back(render::RenderEdge(x_min, y_max, x_min, y_min));

    contour_bitmap->render_contour_ortho(ortho_edges);
}

DrawBitmapVisitor::DrawBitmapVisitor(unsigned long  levels,
                                     int block_index,
                                     OasisTrans dbu_trans,
//                                    std::map<LDType, int>& lds,
                                    std::unordered_map<LDType, int, render::hash_LDType, render::cmp_LDType>& lds,
                                    std::vector<render::Bitmap*>&bitmaps,
                                    render::CellCacheManager* cache,
                                    render::RenderThread* thread):
                                    m_block_index(block_index),
                                    m_dbu_trans(dbu_trans),
                                    m_ld_map(lds),
                                    m_bitmaps(bitmaps),
                                    m_cache(cache),
                                    m_thread(thread),
                                    planes_per_layer(3),
                                    m_fast_draw_array_times(0),
                                    m_normal_render_times(0),
                                    m_fast_draw_cell_times(0),
                                    m_single_draw_polygon_times(0),
                                    m_single_draw_box_times(0),
                                    m_repetition_times(0),
                                    m_draw_dot_times(0),
                                    m_draw_out_of_box_times(0),
                                    m_draw_box_times(0),
                                    m_elements(0),
                                    m_draw_instances(true)
{
    if(levels > 100)
    {
        m_levels = 10;
    }
    else if(levels == 0)
    {
        m_levels = 1;
    }
    else
    {
        m_levels = (unsigned long) std::sqrt(levels);
    }
}


void DrawBitmapVisitor::draw_cell_box(oasis::Box& box, oasis::OasisTrans& trans, render::Bitmap* contour_bitmap, render::Bitmap* fill_bitmap)
{
    if(box.is_empty())
    {
       return ;
    }

    BoxF transed_box = box.transedF(trans);

    double threshold = 1.0 / fabs(trans.mag());
    if (box.width() < threshold && box.height() < threshold)
    {
        PointF center(transed_box.left() + transed_box.width() / 2, transed_box.bottom() + transed_box.height() / 2);
        draw_dot(center, contour_bitmap, fill_bitmap);
        return ;
    }

    float64 x_min = transed_box.left();
    float64 x_max = transed_box.right();
    float64 y_min = transed_box.bottom();
    float64 y_max = transed_box.top();

    int64 height = fill_bitmap->height();
    int64 width = fill_bitmap->width();


    x_min += 0.5;
    x_max += 0.5;
    y_min += 0.5;
    y_max += 0.5;

    if(x_max < 0.0 || x_min >= width || y_max < 0.0 || y_min >= height)
    {
        return ;
    }

    unsigned int y1 = (unsigned int) (std::max(0.0, std::min(y_min, double(height - 1))));
    unsigned int y2 = (unsigned int) (std::max(0.0, std::min(y_max, double(height - 1))));
    unsigned int x1 = (unsigned int) (std::max(0.0, std::min(x_min, double(width - 1))));
    unsigned int x2 = (unsigned int) (std::max(0.0, std::min(x_max, double(width - 1))));

//    m_draw_cell_box.push_back(Box(x1, y1, x2, y2));
    for(unsigned int i = y1; i <= y2; i++)
    {
        fill_bitmap->fill(i, x1, x2 + 1);
        contour_bitmap->fill(i, x1, x2 + 1);
    }
}


void DrawBitmapVisitor::draw_polygon(oasis::Polygon& polygon, oasis::Box& bbox, oasis::OasisTrans& trans, render::Bitmap* contour_bitmap, render::Bitmap* fill_bitmap)
{
    BoxF transed_box = bbox.transedF(trans);
    double threshold = 1.0 / fabs(trans.mag());
    if (bbox.width() < threshold && bbox.height() < threshold)
    {
        PointF center(transed_box.left() + transed_box.width() / 2, transed_box.bottom() + transed_box.height() / 2);
        draw_dot(center, contour_bitmap, fill_bitmap);
        return ;
    }

    float64 x_min = transed_box.left();
    float64 x_max = transed_box.right();
    float64 y_min = transed_box.bottom();
    float64 y_max = transed_box.top();

    int64 height = fill_bitmap->height();
    int64 width = fill_bitmap->width();

    if(x_max < -0.5 || x_min + 0.5 > width|| y_max < -0.5 || y_min + 0.5 > height)
    {
        return ;
    }

    if(enable_simplify_box(bbox, trans))
    {
        draw_box(bbox, trans, contour_bitmap, fill_bitmap);
    }
    else
    {
        if(floor(x_max + 0.5) == floor(x_min + 0.5))
        {
            unsigned int y1 = (unsigned int) (std::max(0.0, std::min(y_min + 0.5, double(height - 1))));
            unsigned int y2 = (unsigned int) (std::max(0.0, std::min(y_max + 0.5, double(height - 1))));
            unsigned int x = (unsigned int) (std::max(0.0, std::min(x_min + 0.5, double(width - 1))));
            for(unsigned int i = y1; i <= y2; i++)
            {
                contour_bitmap->fill(i, x, x + 1);
                fill_bitmap->fill(i, x, x + 1);
            }
            return ;
        }

        if(floor(y_max + 0.5) == floor(y_min + 0.5))
        {
            unsigned int x1 = (unsigned int) (std::max(0.0, std::min(x_min + 0.5, double(width - 1))));
            unsigned int x2 = (unsigned int) (std::max(0.0, std::min(x_max + 0.5, double(width - 1))));
            unsigned int y = (unsigned int) (std::max(0.0, std::min(y_min + 0.5, double(height - 1))));
            contour_bitmap->fill(y, x1, x2 + 1);
            fill_bitmap->fill(y, x1, x2 + 1);
            return ;
        }

        std::vector<render::RenderEdge> edges;
        std::vector<render::RenderEdge> ortho_edges;
        std::vector<Point> points = polygon.get_points();

        PointF prev(trans.transF(PointF(points.back())));
        PointF current;

        for(uint32 i = 0; i < points.size(); i++)
        {
           current = trans.transF(PointF(points[i]));
           render::RenderEdge edge(prev.x(), prev.y(), current.x(), current.y());
           if(edge.is_horizontal()|| edge.is_vertical())
           {
               ortho_edges.push_back(edge);
           }
           else
           {
               edges.push_back(edge);
           }
           prev = current;
        }

        if(!ortho_edges.empty())
        {
            contour_bitmap->render_contour_ortho(ortho_edges);
        }
        if(!edges.empty())
        {
            contour_bitmap->render_contour(edges);
        }

        std::vector<render::RenderEdge> result;
        result.insert(result.end(), ortho_edges.begin(), ortho_edges.end());
        result.insert(result.end(), edges.begin(), edges.end());

        if(!result.empty())
        {
            fill_bitmap->render_fill(result);
        }
    }
}


bool DrawBitmapVisitor::draw_polygon_array(oasis::Box& box, oasis::Box& box_with_repetition, oasis::Box&qbox, oasis::OasisTrans& trans,
                               std::vector<Point>& disps, render::Bitmap* contour_bitmap, render::Bitmap* fill_bitmap)
{
    BoxF bbox = box;
    Box bbox_with_repetition = box_with_repetition;
    bbox.trans(trans);
    bbox_with_repetition.trans(trans);
    int dots = bbox_with_repetition.width() * bbox_with_repetition.height();
    if(disps.size() < (size_t)dots)
    {
        return false;
    }
    if(bbox.width() < 1.5  && bbox.height() < 1.5)
    {
        if((bbox_with_repetition.height() < 3.0 * 10.0 / m_levels && bbox_with_repetition.width() < 7.0 * 10.0 / m_levels) ||
           (bbox_with_repetition.height() < 7.0 * 10.0 / m_levels && bbox_with_repetition.width() < 3.0 * 10.0 / m_levels))
        {
            BoxF transed_box = box_with_repetition.transedF(trans);

            double threshold = 1.0 / fabs(trans.mag());
            if (box_with_repetition.width() < threshold && box_with_repetition.height() < threshold)
            {
                PointF center(transed_box.left() + transed_box.width() / 2, transed_box.bottom() + transed_box.height() / 2);
                draw_dot(center, contour_bitmap, fill_bitmap);
                m_draw_dot_times++;
                return true;
            }

            float64 x_min = transed_box.left();
            float64 x_max = transed_box.right();
            float64 y_min = transed_box.bottom();
            float64 y_max = transed_box.top();

            int64 height = fill_bitmap->height();
            int64 width = fill_bitmap->width();


            x_min += 0.5;
            x_max += 0.5;
            y_min += 0.5;
            y_max += 0.5;

            if(x_max < 0.0 || x_min >= width || y_max < 0.0 || y_min >= height)
            {
                m_draw_out_of_box_times++;
                return true;
            }

            m_draw_box_times++;
            unsigned int y1 = (unsigned int) (std::max(0.0, std::min(y_min, double(height - 1))));
            unsigned int y2 = (unsigned int) (std::max(0.0, std::min(y_max, double(height - 1))));
            unsigned int x1 = (unsigned int) (std::max(0.0, std::min(x_min, double(width - 1))));
            unsigned int x2 = (unsigned int) (std::max(0.0, std::min(x_max, double(width - 1))));

            for(unsigned int i = y1; i <= y2; i++)
            {
                fill_bitmap->fill(i, x1, x2 + 1);
                contour_bitmap->fill(i, x1, x2 + 1);
            }
            return true;
        }

    }
    return false;
}

//static std::vector<LDType> get_layers(const OasisCell& cell, std::map<LDType, int>& ld_map)
//{
//    std::vector<LDType> result_list;
//    for(auto it = ld_map.begin(); it != ld_map.end(); it++)
//    {
//        if(cell.has_layer((it->first).layer, (it->first).datatype))
//        {
//            result_list.push_back(it->first);
//        }
//    }
//    return result_list;
//}


static std::vector<LDType> get_layers(const OasisCell& cell, std::unordered_map<LDType, int, render::hash_LDType, render::cmp_LDType>&ld_map)
{
    std::vector<LDType> result_list;
    for(auto it = ld_map.begin(); it != ld_map.end(); it++)
    {
        if(cell.has_layer((it->first).layer, (it->first).datatype))
        {
            result_list.push_back(it->first);
        }
    }
    return result_list;
}

static bool has_layer(const OasisCell& cell, std::map<LDType, int>& ld_map)
{
    for(auto it = ld_map.begin(); it != ld_map.end(); it++)
    {
        if(cell.has_layer((it->first).layer, (it->first).datatype))
        {
            return true;
        }
    }
    return false;
}


//void DrawBitmapVisitor::fast_draw_cell(render::CellCacheStatus& status, OasisTrans& trans)
//{
//    m_fast_draw_cell_times++;
//    status.m_hits++;
//    Point p = Point(status.m_disp + trans.dispF());
//    cp_bitmap(status.m_contour_groups, 0 ,p.x(), p.y());
//    cp_bitmap(status.m_fill_groups, 1, p.x(), p.y());
//}

void DrawBitmapVisitor::visit_instances(const std::vector<OasisElement*> &elements)
{
    m_thread->sentinel();
    const OasisTrans& trans = m_trans.top();
    const Box& qbox = m_query_boxes.top();
    for (uint32 i = 0; i < elements.size(); ++i)
    {
        PlacementElement* p = dynamic_cast<PlacementElement*>(elements[i]);
        oas_assert(p != NULL);
        std::vector<Point> disps;
        p->get_disps(disps);
        OasisTrans itrans = p->get_trans();
        OasisCellInfo& cellinfo = m_layout->get_cell_info(p->get_info_index());
        OasisCell& cell = m_layout->get_cell(cellinfo.get_cell_index());

        Box cell_box = cell.get_bbox();
        BoxF cell_fbox(cell_box);
        OasisTrans tmp = trans * itrans;
        BoxF pixel_fbox = cell_fbox.transed(tmp);

        std::vector<LDType> layers_in_cell = get_layers(cell, m_ld_map);
        if(layers_in_cell.empty())
        {
            continue ;
        }

        if(is_small_cell(pixel_fbox))
        {
            for(uint32 k = 0; k < disps.size(); ++k)
            {
                OasisTrans disp(false, 0, disps[k]);
                disp = disp * itrans;
                OasisTrans sub_trans = trans * disp;
                Box sub_qbox = qbox.transed(disp.inverted());
                if(!sub_qbox.overlap(cell_box))
                {
                    continue;
                }
                sub_qbox.intersect(cell_box);

                for(size_t j = 0; j < layers_in_cell.size(); j++)
                {
                    int layer_index = m_ld_map[layers_in_cell[j]];
                    render::Bitmap* contour_bitmap = m_bitmaps[layer_index * planes_per_layer];
                    render::Bitmap* fill_bitmap = m_bitmaps[layer_index * planes_per_layer + 1];
                    draw_cell_box(sub_qbox, sub_trans, contour_bitmap, fill_bitmap);
                }
                m_fast_draw_cell_times++;
                m_fast_draw_cell_name.insert(cellinfo.get_name());
            }
            continue ;
        }

        float64 threshold = 5.0 * ( 1.1 - 0.1 * m_levels) / tmp.mag();
        if((cell_box.width() < threshold && cell_box.height() < threshold))
        {
            for(uint32 k = 0; k < disps.size(); ++k)
            {
                OasisTrans disp(false, 0, disps[k]);
                disp = disp * itrans;
                OasisTrans sub_trans = trans * disp;
                Box sub_qbox = qbox.transed(disp.inverted());
                if(!sub_qbox.overlap(cell_box))
                {
                    continue;
                }
                for(size_t j = 0; j < layers_in_cell.size(); j++)
                {
                    int layer_index = m_ld_map[layers_in_cell[j]];
                    render::Bitmap* contour_bitmap = m_bitmaps[layer_index * planes_per_layer];
                    render::Bitmap* fill_bitmap = m_bitmaps[layer_index * planes_per_layer + 1];
                    draw_cell_box(cell_box, sub_trans, contour_bitmap, fill_bitmap);
                }
                m_fast_draw_cell_times++;
                m_fast_draw_cell_name.insert(cellinfo.get_name());
            }
            continue ;
        }

        for(uint32 k = 0; k < disps.size(); ++k)
        {
            OasisTrans disp(false, 0, disps[k]);
            disp = disp * itrans;
            m_trans.push(trans * disp);
            m_query_boxes.push(qbox.transed(disp.inverted()));
            cell.accept_visitor(*this);

            m_query_boxes.pop();
            m_trans.pop();
        }

    }
}

void DrawBitmapVisitor::visit_geometries(const std::vector<OasisElement*> &elements)
{
    m_thread->sentinel();
    int level = 5;
    OasisTrans& trans = m_trans.top();
    Box qbox = m_query_boxes.top();

    normal_render(elements, trans, qbox, level);

////    m_draw_instances = true;
////    OasisCellInfo& cellinfo = m_layout->get_cell_info(m_info_index);
////    OasisCell& cell = m_layout->get_cell(cellinfo.get_cell_index());
////    m_normal_draw_cell_name.insert(cellinfo.get_name());


////    {
////        bool use_cache = true;

////        if(!inside(cell_box, qbox))
////        {
////            use_cache = false;
////        }

////        if(use_cache)
////        {
////            OasisTrans trans_no_disp = trans;
////            trans_no_disp.set_disp(PointF());
////            render::CellKey key(trans_no_disp, cellinfo.get_cell_index());
////            if(!m_cache->find_key(key))
////            {
////                render::CellCacheStatus status;
////                BoxF cell_box_trans = cell_fbox.transedF(trans_no_disp);

////                PointF d(floor(cell_box_trans.left() + trans.dispF().x()),
////                         floor(cell_box_trans.bottom() + trans.dispF().y()));

////                status.m_disp = PointF(d.x() - trans.dispF().x(),
////                                                   d.y() - trans.dispF().y());


////                OasisTrans render_trans = trans_no_disp;
////                render_trans.set_disp(PointF() - status.m_disp);

////                int width = int(cell_box_trans.width() + 3);
////                int height = int(cell_box_trans.height() + 3);

////                for(size_t i = 0; i < layers_in_cell.size(); i++)
////                {
////                    LDType ld = layers_in_cell[i];
////                    status.m_fill_groups[ld] =  render::BitmapInfo(new render::Bitmap(width, height, 1.0), m_ld_map[ld]);
////                    status.m_contour_groups[ld] = render::BitmapInfo(new render::Bitmap(width, height, 1.0), m_ld_map[ld]);
////                }

////                initialize_cache(elements, render_trans, qbox, status, level);
////                fast_draw_cell(status, trans);
////                m_cache->set(key, status);
////            }
////            else
////            {
////                render::CellCacheStatus& status = m_cache->get(key);
////                fast_draw_cell(status, trans);
////            }
////        }
////        else
////        {
////            normal_render(elements, trans, qbox, level);
////        }
////    }
}

//void DrawBitmapVisitor::cp_bitmap(std::map<LDType, render::BitmapInfo>& bitmap_groups, int offset, int x, int y)
//{
//    for(auto it = bitmap_groups.begin(); it != bitmap_groups.end(); it++)
//    {
//        render::Bitmap* bitmap = (it->second).bitmap;
//        if(bitmap)
//        {
//            render::Bitmap* bitmap_to = m_bitmaps[(it->second).layer_index * planes_per_layer + offset];
//            bitmap_to->merge(bitmap, x, y);
//        }
//    }
//}

//void DrawBitmapVisitor::initialize_cache(const std::vector<OasisElement*> & elements, OasisTrans& trans, Box& qbox, render::CellCacheStatus& status, int levels)
//{
//    for(uint32 i = 0; i < elements.size(); i++)
//    {
//        OasisElement* e = elements[i];
//        oasis::LDType ld(e->layer(), e->datatype());
//        auto it = m_ld_map.find(ld);
//        if(it == m_ld_map.end())
//        {
//            continue;
//        }

//        Polygon polygon;
//        Box box;
//        Box box_with_repetition;
//        std::vector<Point> disps;
//        e->get_disps(disps);
//        e->polygon(polygon);
//        e->bbox(box);
//        e->get_bbox_with_repetition(box_with_repetition);

//        render::Bitmap* contour_bitmap = status.m_contour_groups[ld].bitmap;
//        render::Bitmap* fill_bitmap = status.m_fill_groups[ld].bitmap;

//        if(disps.size() < 2)
//        {
//            if(!qbox.overlap(box))
//            {
//                continue ;
//            }

//            if(enable_simplify_box(box, trans))
//            {
//                draw_box(box, trans, contour_bitmap, fill_bitmap);
//            }
//            else
//            {
//                m_single_draw_polygon_times++;
//                draw_polygon(polygon, box, trans, contour_bitmap, fill_bitmap);
//            }
//        }
//        else
//        {
//            if(!draw_polygon_array(box, box_with_repetition, qbox, trans, disps, contour_bitmap, fill_bitmap))
//            {
//                m_normal_render_times++;
////                m_fast_block_times += disps.size();
//                for(size_t k = 0; k < disps.size(); k++)
//                {
//                    Box b = box;
//                    b.shift(disps[k]);

//                    if(!qbox.overlap(b))
//                    {
//                        continue;
//                    }
//                    Polygon p = polygon.shifted(disps[k]);
//                    draw_polygon(p, b , trans, contour_bitmap, fill_bitmap);
//                }

//            }
////            else
////            {
////                m_fast_draw_cell_times++;
////            }

//        }
//    }
//}


void DrawBitmapVisitor::normal_render(const std::vector<OasisElement*> &elements,
                                      OasisTrans& trans,
                                      Box& qbox,
                                      int levels)
{
    for(uint32 i = 0; i < elements.size(); i++)
    {
        OasisElement* e = elements[i];
        oasis::LDType ld(e->layer(), e->datatype());
        auto it = m_ld_map.find(ld);
        m_elements++;
        if(it == m_ld_map.end())
        {
            continue;
        }
        int layer_index = it->second;
        Polygon polygon;
        Box box;
        Box box_with_repetition;
        std::vector<Point> disps;
        e->get_disps(disps);
        e->polygon(polygon);
        e->bbox(box);
        e->get_bbox_with_repetition(box_with_repetition);
        render::Bitmap* contour_bitmap = m_bitmaps[layer_index * planes_per_layer];
        render::Bitmap* fill_bitmap = m_bitmaps[layer_index * planes_per_layer + 1];

        if(!box_with_repetition.overlap(qbox))
        {
            continue;
        }

        box_with_repetition.intersect(qbox);

        if(!draw_polygon_array(box, box_with_repetition, qbox, trans, disps, contour_bitmap, fill_bitmap))
        {
            for(size_t k = 0; k < disps.size(); k++)
            {
                m_normal_render_times += 1;
                Box b = box;
                b.shift(disps[k]);

                if(!qbox.overlap(b))
                {
                    continue;
                }
                Polygon p = polygon.shifted(disps[k]);
                draw_polygon(p, b , trans, contour_bitmap, fill_bitmap);
            }

        }
        else
        {
            m_fast_draw_array_times++;
        }
    }
}

void DrawBitmapVisitor::print_hits(int block_index)
{
//    std::string result_1 = "";
//    std::string result_2 = "";
//    for(auto it = m_fast_draw_cell_name.begin(); it != m_fast_draw_cell_name.end(); it++)
//    {
//        result_1 += *it;
//        result_1 += " ";
//    }
//    for(auto it = m_normal_draw_cell_name.begin(); it != m_normal_draw_cell_name.end(); it++)
//    {
//        result_2 += *it;
//        result_2 += " ";

//    }
//    std::string result_3;
//    for(auto it = m_out_of_box.begin(); it != m_out_of_box.end(); it++)
//    {
//        std::stringstream ss;
//        BoxF box = it->first;
//        Point size = it->second;
//        ss << "box:";
//        ss << "left:" << box.left() << " right:" << box.right() << " bottom:" << box.bottom() << "top" << box.top();
//        ss << " width:" << size.x() << " height" << size.y();
//        ss << " ";
//        result_3 += ss.str();
//    }
//    std::string result_4;
//    for(auto it = m_draw_cell_box.begin(); it != m_draw_cell_box.end(); it++)
//    {
//        std::stringstream ss;
//        BoxF box = *it;
//        ss << "box:";
//        ss << "left:" << box.left() << " right:" << box.right() << " bottom:" << box.bottom() << "top" << box.top();
//        ss << " ";
//        result_4 += ss.str();
//    }
//    qDebug() << "block index" << block_index
//             << "zoom in levels" << m_levels
//////             << "fast draw cell" << QString::fromStdString(result_1) << "\n"
//////             << "normal draw cell" <<QString::fromStdString(result_2) << "\n"
//////             << "out_of_box info:" << QString::fromStdString(result_3) << "\n"
//////             << "draw_cell_box info:" << QString::fromStdString(result_4) << "\n"
//             << "fast draw cell times" << m_fast_draw_cell_times
//             << "elements num" << m_elements
//             << "normal render times" << m_normal_render_times
//             << "fast draw array times" << m_fast_draw_array_times
////             << "single draw polygon" << m_single_draw_polygon_times
////             << "single draw box" << m_single_draw_box_times
//             << "draw_dots:" << m_draw_dot_times
//             << "out of box" << m_draw_out_of_box_times
//             << "draw_box" << m_draw_box_times;
}

