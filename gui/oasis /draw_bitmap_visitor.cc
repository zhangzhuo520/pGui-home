
#include <QDebug>

#include "draw_bitmap_visitor.h"
#include "oasis_cell.h"
#include "render_bitmap.h"

#include "stdint.h"

using namespace oasis;

static const double dbu = 1e-4;

static bool check_ld(int32 layer, int32 datatype, const LDType& ld)
{
    if(ld.layer == layer && ld.datatype == datatype)
    {
        return true;
    }
    return false;
}

void DrawBitmapVisitor::visit_instances(const std::vector<OasisElement*> &elements)
{
    CellVisitor::visit_instances(elements); //TODO to improve
}

void DrawBitmapVisitor::visit_geometries(const std::vector<OasisElement*> &elements)
{
    Box qbox = m_query_boxes.top();
    OasisTrans& trans = m_trans.top();
    std::vector<Polygon> m_results;
    for(uint32 i = 0 ; i< elements.size(); i++)
    {
        OasisElement* e = elements[i];
        if(!check_ld(e->layer(), e->datatype(), m_ld))
        {
            continue;
        }

        Polygon polygon;
        Box box;

        std::vector<Point> disps;
        e->get_disps(disps);
        e->polygon(polygon);
        e->bbox(box);

        for(uint32 k = 0; k < disps.size(); k++)
        {
            Box b = box;
            b.shift(disps[k]);
            if(!qbox.overlap(b))
            {
                continue;
            }

            Polygon p = polygon.shifted(disps[k]);

            b.trans(trans);
            if(b.width() <= 2 && b.height() <= 2)
            {
                int64 top = m_height - 1;
                int64 right = m_width - 1;
                m_contour->fill(std::max(0L, std::min(b.bottom(), top)),
                                std::min(right, std::max(0L, b.left())),
                                std::max(0L, std::min(b.left() + 1, right)));

                m_fill->fill(std::max(0L, std::min(b.bottom(), top)),
                             std::min(right, std::max(0L, b.left())),
                             std::max(0L, std::min(b.left() + 1, right)));
                continue;
            }
            else
            {
                m_results.push_back(p);
            }
        }


    }


    std::vector<render::RenderEdge> edges;
    std::vector<render::RenderEdge> ortho_edges;
    for(uint32 j = 0; j < m_results.size(); j++)
    {
        Polygon polygon = m_results[j];
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
    }

    if(!ortho_edges.empty())
    {
        m_contour->render_contour_ortho(ortho_edges);
    }
    if(!edges.empty())
    {
        m_contour->render_contour(edges);
    }
    std::vector<render::RenderEdge> result;
    result.insert(result.end(), ortho_edges.begin(), ortho_edges.end());
    result.insert(result.end(), edges.begin(), edges.end());

    if(!result.empty())
    {
        m_fill->render_fill(result);
    }
}
