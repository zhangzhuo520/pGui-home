#ifndef SNAPEDGEVISITOR_CC
#define SNAPEDGEVISITOR_CC
#include "snap_edge_visitor.h"
#include "oasis_cell.h"
#include <QDebug>

using namespace oasis;

static bool check_ld(int32 layer, int32 datatype, const LDType& ld)
{
    if(ld.layer == layer && ld.datatype == datatype)
    {
        return true;
    }
    return false;
}

static double vec_product(double ax, double ay,
                          double bx, double by,
                          double cx, double cy)
{
    double ca_x = ax - cx;
    double ca_y = ay - cy;
    double cb_x = bx - cx;
    double cb_y = by - cy;
    double result = ca_x * cb_y - ca_y * cb_x;
    return result;
}

static double length(EdgeF& e)
{
    double dx = e.p1().x() - e.p2().x();
    double dy = e.p1().y() - e.p2().y();
    return sqrt(dx * dx + dy * dy);
}

static double distance_closest(EdgeF e, PointF &p)
{
    if(e.p1() == e.p2())
    {
        return 0;
    }

    double area = vec_product(e.p2().x(), e.p2().y(), p.x(), p.y(), e.p1().x(), e.p1().y());
    double d = fabs(double (area)) / length(e);
    return d;
}

// vector ca * vector cb
static int scalar_product_sign(double ax, double ay, double bx, double by, double cx, double cy)
{
    double dx_ca = ax - cx;
    double dy_ca = ay - cy;
    double dx_cb = bx - cx;
    double dy_cb = by - cy;

    double product = dx_ca * dx_cb + dy_ca * dy_cb;
    double threshold = (sqrt(dx_ca * dx_ca + dy_ca * dy_ca) + sqrt(dx_cb * dx_cb + dy_cb * dy_cb) )* 1e-5;

    if(product < -threshold)
    {
        return -1;
    }
    else if(product < threshold)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/*calculate vec CA cross vec CB
*/
static int vec_cross_sign(double ax, double ay,
                          double bx, double by,
                          double cx, double cy)
{
    double ca_x = ax - cx;
    double ca_y = ay - cy;
    double cb_x = bx - cx;
    double cb_y = by - cy;
    double result = ca_x * cb_y - ca_y * cb_x;
    double threshold = (sqrt(ca_x * ca_x + ca_y * ca_y) + sqrt(cb_x *cb_x + cb_y * cb_y))*1e-5;
    if(result > threshold)
    {
        return 1;
    }
    else if(result > -threshold)
    {
        return 0;
    }
    else
    {
        return -1;
    }

}


static bool contains(EdgeF e, PointF p)
{
    if(e.p1() == e.p2())
    {
        return e.p1() == p;
    }
    else
    {
        return distance_closest(e, p) < 1e-5 && (scalar_product_sign(p.x(), p.y(), e.p2().x(), e.p2().y(), e.p1().x(), e.p1().y())) >= 0 &&
               (scalar_product_sign(p.x(), p.y(), e.p1().x(), e.p1().y(), e.p2().x(), e.p2().y())) >= 0;

    }
    return false;

}


static bool contains(BoxF box, PointF p)
{
    if(box.is_empty())
    {
        return false;
    }

    return p.x() >= box.left() && p.x() <= box.right() &&
           p.y()>= box.bottom() && p.y() <= box.top();
}

static bool clipped(BoxF& box, EdgeF& edge)
{
    if(box.is_empty())
    {
        return false;
    }
    PointF p1 = edge.p1();
    PointF p2 = edge.p2();

    if(p1.x() > p2.x())
    {
        PointF tmp = p1;
        p1 = p2;
        p2 = tmp;
    }

    if(p2.x() < box.left())
    {
        return false;
    }
    if(p1.x() > box.right())
    {
        return false;
    }

    if(p1.y() > p2.y())
    {
        PointF tmp = p1;
        p1 = p2;
        p2 = tmp;
    }

    if(p1.y() > box.top())
    {
        return false;
    }

    if(p2.y() < box.bottom())
    {
        return false;
    }
    return true;

}

static bool is_crossed(EdgeF e1, EdgeF e2)
{
    bool result = true;

    int sign1 = vec_cross_sign(e1.p2().x(), e1.p2().y(), e2.p1().x(), e2.p1().y(), e1.p1().x(), e1.p1().y());
    if(sign1 < 0)
    {
        result = false;
    }
    else if(sign1 == 0)
    {
        return true;
    }

    int sign2 = vec_cross_sign(e1.p2().x(), e1.p2().y(), e2.p2().x(), e2.p2().y(), e1.p1().x(), e1.p1().y());
    if(sign2 > 0)
    {
        result = !result;
    }
    else if(sign2 == 0)
    {
        return true;
    }
    return result;
}

static bool is_horizontal(EdgeF e)
{
    return e.p1().y() == e.p2().y();
}

static bool is_vertical(EdgeF e)
{
    return e.p1().x() == e.p2().x();
}

static bool is_touches(BoxF box1, BoxF box2)
{
    if(box1.is_empty() || box2.is_empty())
    {
        return false;
    }
    else
    {
        return (box1.left() <= box2.right() && box2.left() <= box1.right()) &&
               (box1.bottom() <= box2.top() && box2.bottom() <= box1.top());
    }
}

// e1: edge on polygon
// e2: projected edges

static std::pair<bool, PointF> intersected(EdgeF e1, EdgeF e2, PointF e2_center)
{
    float64 left  = std::min(e1.p1().x(), e1.p2().x());
    float64  right = std::max(e1.p1().x(), e1.p2().x());
    float64  bot   = std::min(e1.p1().y(), e1.p2().y());
    float64  top   = std::max(e1.p1().y(), e1.p2().y());

    BoxF box1(left, bot, right , top);

    left  = std::min(e2.p1().x(), e2.p2().x());
    right = std::max(e2.p1().x(), e2.p2().x());
    bot   = std::min(e2.p1().y(), e2.p2().y());
    top   = std::max(e2.p1().y(), e2.p2().y());

    BoxF box2(left, bot, right, top);

    if(!is_touches(box1, box2))
    {
        return std::make_pair(false, PointF());
    }

    if(is_vertical(e1) && is_horizontal(e2))
    {
        //qDebug() << "v & h";
        return std::make_pair(true, PointF(e1.p1().x(), e2.p1().y()));
    }

    if(is_horizontal(e1) && is_vertical(e2))
    {
        //qDebug() << "h & v";
        return std::make_pair(true, PointF(e2.p1().x(), e1.p1().y()));
    }

    if(!is_crossed(e1, e2) || !is_crossed(e2, e1))
    {
        //qDebug() << "not crossed.";
        return std::make_pair(false, PointF());
    }

    bool result = true;
    bool on_edge = false;

    float64  a_x = e2.p2().x();
    float64  a_y = e2.p2().y();
    float64  b_x = e1.p1().x();
    float64  b_y = e1.p1().y();
    float64  c_x = e2.p1().x();
    float64  c_y = e2.p1().y();

    float64 product1 = (a_x - c_x) * (b_y - c_y);
    float64 product2 = (a_y - c_y) * (b_x - c_x);
    float64 vec_p1 = product1 -product2;
    if(product1 < product2)
    {
        result = false;
    }
    else if(product1 == product2)
    {
        on_edge = true;
    }

    b_x = e1.p2().x();
    b_y = e1.p2().y();
    product1 = (a_x - c_x) * (b_y - c_y);
    product2 = (a_y - c_y) * (b_x - c_x);
    float64 vec_p2 = product1 - product2;
    if(product1 > product2)
    {
        result = !result;
    }
    else if(product1 == product2)
    {
        on_edge = true;
    }

    if(on_edge)
    {
        if(contains(e1, e2_center))
        {
            return std::make_pair(true, e2_center);
        }
        return std::make_pair(false, PointF());
    }

    if(result)
    {
        float64 ratio = fabs(float64(vec_p1)) / (fabs(float64(vec_p1)) + fabs(float64(vec_p2)));
        float64 x = e1.p1().x() +  (e1.p2().x() - e1.p1().x()) * ratio;
        float64 y = e1.p1().y() +  (e1.p2().y() - e1.p1().y()) * ratio;
        return std::make_pair(true, PointF(x, y));
    }
    else{
        return std::make_pair(false,PointF());
    }

}

static float64 distance(PointF p1, PointF p2)
{
    float64 dx = p2.x() - p1.x();
    float64 dy = p2.y() - p1.y();
    return sqrt(dx * dx + dy * dy);
}

void SnapEdgeVisitor::visit_instances(const std::vector<OasisElement*> &elements)
{
    CellVisitor::visit_instances(elements);
}


void SnapEdgeVisitor::visit_geometries(const std::vector<OasisElement*> &elements)
{
    Box& qbox = m_query_boxes.top();
    for(uint32 i = 0; i < elements.size(); i++)
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

        for(uint32 k = 0; k < disps.size() ;k++)
        {
            Box b = box;
            b.shift(disps[k]);

            if(!qbox.overlap(b))
            {
                continue;
            }
            Polygon p = polygon.shifted(disps[k]);
            find_closest_point(p);
        }
    }
}

void SnapEdgeVisitor::find_closest_point(Polygon& polygon)
{
    std::vector<Point> points = polygon.get_points();
    OasisTrans& trans = m_trans.top();

    PointF prev = trans.transF(PointF(points.back()));
    prev.set_x(prev.x() * m_dbu);
    prev.set_y(prev.y() * m_dbu);
    PointF current;

    for(uint32 i = 0; i < points.size(); i++)
    {
        current = trans.transF(PointF(points[i]));
        current.set_x(current.x() * m_dbu);
        current.set_y(current.y() * m_dbu);
        EdgeF e (prev, current);
        check_edge(e);
        prev = current;
    }
}


void SnapEdgeVisitor::check_edge(EdgeF& e)
{
    if(clipped(m_micron_box, e))
    {
        search_closest(e);
    }
}


void SnapEdgeVisitor::search_closest(EdgeF& e)
{
    if(m_snap_mode == Angle_Any)
    {
        if(contains(m_micron_box, e.p1()))
        {
            compare_closest_exact(e.p1(), e);
        }

        if(contains(m_micron_box, e.p2()))
        {
            compare_closest_exact(e.p2(), e);
        }

        float64 length = std::max(m_micron_box.width(), m_micron_box.height());
        PointF v(e.p2().x() - e.p1().x(), e.p2().y() - e.p1().y());
        PointF n(-v.y(), v.x());

        float64 n_length = sqrt(n.x() * n.x() + n.y()  *n.y());
        float64 f = length/ n_length;

        PointF p1(m_center.x() - n.x() * f, m_center.y() - n.y() * f);
        PointF p2(m_center.x() + n.x() * f, m_center.y() + n.y() * f);

        std::pair<bool, PointF> result = intersected(e, EdgeF(p1, p2), m_center);
        if(result.first)
        {
            compare_closest(result.second, e);
        }

    }
    else if(m_snap_mode == Angle_Horizontal)
    {

        float64 length = std::max(m_micron_box.width(), m_micron_box.height());
        PointF v(e.p2().x() - e.p1().x(), e.p2().y() - e.p1().y());
        PointF n(-v.y(), v.x());

        float64 n_length = sqrt(n.x() * n.x() + n.y()  *n.y());
        float64 f = length/ n_length;

        PointF p1(m_center.x() - n.x() * f, m_center.y() - n.y() * f);
        PointF p2(m_center.x() + n.x() * f, m_center.y() + n.y() * f);
        EdgeF e2(p1, p2);

        std::pair<bool, PointF> result = intersected(e, e2, m_center);

        PointF cutline_p1 = PointF(m_center.x() - length, m_center.y());
        PointF cutline_p2 = PointF(m_center.x() + length, m_center.y());
        EdgeF cutline = EdgeF(cutline_p1, cutline_p2);

        if(result.first && contains(cutline, result.second))
        {
            compare_closest(result.second, e);
        }

        result = intersected(e, cutline, m_center);
        if(result.first && contains(cutline, result.second) )
        {
            compare_closest(result.second, e);
        }

        if(contains(m_micron_box, e.p1()) && contains(cutline, e.p1()))
        {
            compare_closest_exact(e.p1(), e);
        }

        if(contains(m_micron_box, e.p2()) && contains(cutline, e.p2()))
        {
            compare_closest_exact(e.p2(), e);
        }

    }
    else if(m_snap_mode == Angle_Vertical)
    {
        float64 length = std::max(m_micron_box.width(), m_micron_box.height());
        PointF v(e.p2().x() - e.p1().x(), e.p2().y() - e.p1().y());
        PointF n(-v.y(), v.x());

        float64 n_length = sqrt(n.x() * n.x() + n.y()  *n.y());
        float64 f = length/ n_length;

        PointF p1(m_center.x() - n.x() * f, m_center.y() - n.y() * f);
        PointF p2(m_center.x() + n.x() * f, m_center.y() + n.y() * f);
        EdgeF e2(p1, p2);

        std::pair<bool, PointF> result = intersected(e, e2, m_center);

        PointF cutline_p1 = PointF(m_center.x(), m_center.y() - length);
        PointF cutline_p2 = PointF(m_center.x(), m_center.y() + length);
        EdgeF cutline = EdgeF(cutline_p1, cutline_p2);

        if(result.first && contains(cutline, result.second))
        {
            compare_closest(result.second, e);
        }

        // cutline snapping
        result = intersected(e, cutline, m_center);
        if(result.first && contains(cutline, result.second) )
        {
            compare_closest(result.second, e);
        }

        if(contains(m_micron_box, e.p1()) && contains(cutline, e.p1()))
        {
            compare_closest_exact(e.p1(), e);
        }

        if(contains(m_micron_box, e.p2()) && contains(cutline, e.p2()))
        {
            compare_closest_exact(e.p2(), e);
        }
    }
    else if(m_snap_mode == Angle_Pri_Diagonal)
    {
        float64 length = std::max(m_micron_box.width(), m_micron_box.height());
        PointF v(e.p2().x() - e.p1().x(), e.p2().y() - e.p1().y());
        PointF n(-v.y(), v.x());

        float64 n_length = sqrt(n.x() * n.x() + n.y()  *n.y());
        float64 f = length/ n_length;

        PointF p1(m_center.x() - n.x() * f, m_center.y() - n.y() * f);
        PointF p2(m_center.x() + n.x() * f, m_center.y() + n.y() * f);
        EdgeF e2(p1, p2);

        std::pair<bool, PointF> result = intersected(e, e2, m_center);

        PointF cutline_p1 = PointF(m_micron_box.left(), m_micron_box.top());
        PointF cutline_p2 = PointF(m_micron_box.right(), m_micron_box.bottom());
        EdgeF cutline = EdgeF(cutline_p1, cutline_p2);

        if(result.first && contains(cutline, result.second))
        {
            compare_closest(result.second, e);
        }

        result = intersected(e, cutline, m_center);
        if(result.first && contains(cutline, result.second) )
        {
            compare_closest(result.second, e);
        }

        if(contains(m_micron_box, e.p1()) && contains(cutline, e.p1()))
        {
            compare_closest_exact(e.p1(), e);
        }

        if(contains(m_micron_box, e.p2()) && contains(cutline, e.p2()))
        {
            compare_closest_exact(e.p2(), e);
        }
    }
    else if(m_snap_mode == Angle_Aux_Diagonal)
    {

        float64 length = std::max(m_micron_box.width(), m_micron_box.height());
        PointF v(e.p2().x() - e.p1().x(), e.p2().y() - e.p1().y());
        PointF n(-v.y(), v.x());

        float64 n_length = sqrt(n.x() * n.x() + n.y()  *n.y());
        float64 f = length/ n_length;

        PointF p1(m_center.x() - n.x() * f, m_center.y() - n.y() * f);
        PointF p2(m_center.x() + n.x() * f, m_center.y() + n.y() * f);
        EdgeF e2(p1, p2);

        std::pair<bool, PointF> result = intersected(e, e2, m_center);

        PointF cutline_p1 = PointF(m_micron_box.left(), m_micron_box.bottom());
        PointF cutline_p2 = PointF(m_micron_box.right(), m_micron_box.top());
        EdgeF cutline = EdgeF(cutline_p1, cutline_p2);

        if(result.first && contains(cutline, result.second))
        {
            compare_closest(result.second, e);
        }

        // cutline snapping
        result = intersected(e, cutline, m_center);
        if(result.first && contains(cutline, result.second) )
        {
            compare_closest(result.second, e);
        }

        if(contains(m_micron_box, e.p1()) && contains(cutline, e.p1()))
        {
            compare_closest_exact(e.p1(), e);
        }

        if(contains(m_micron_box, e.p2()) && contains(cutline, e.p2()))
        {
            compare_closest_exact(e.p2(), e);
        }


    }

} 

void SnapEdgeVisitor::compare_closest(const PointF& result_point, const EdgeF& result_edge)
{

    if(!m_found)
    {
        m_found = !m_found;
        m_closest_edge = result_edge;
        m_closest_point = result_point;
        m_closest_distance = std::min(distance(m_closest_point, m_center),m_closest_distance);
    }
    else
    {
        float64 dis = distance(result_point, m_center);
        if( dis < m_closest_distance)
        {
            m_closest_edge = result_edge;
            m_closest_point = result_point;
            m_closest_distance = dis;
        }

    }
}

void SnapEdgeVisitor::compare_closest_exact(const PointF& result_point, const EdgeF& result_edge)
{
    if(!m_found_exact)
    {
        m_found_exact = !m_found_exact;
        m_closest_edge_exact = result_edge;
        m_closest_point_exact = result_point;
        m_closest_distance_exact = std::min(distance(m_closest_point_exact, m_center), m_closest_distance_exact);
    }
    else
    {
        float64 dis = distance(result_point, m_center);
        if(dis < m_closest_distance_exact)
        {
            m_closest_edge_exact = result_edge;
            m_closest_point_exact = result_point;
            m_closest_distance_exact = dis;
        }

    }
}



#endif // SNAPEDGEVISITOR_CC
