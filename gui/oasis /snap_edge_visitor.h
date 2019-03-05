#ifndef SNAPEDGEVISITOR_H
#define SNAPEDGEVISITOR_H
#include "cell_visitor.h"

namespace oasis
{

enum snap_mode_type
{
    Angle_Any = 0,
    Angle_Horizontal,
    Angle_Vertical,
    Angle_Pri_Diagonal, // 135 degree
    Angle_Aux_Diagonal // 45 degree
};

class  SnapEdgeVisitor: public CellVisitor
{

public:
    SnapEdgeVisitor(const LDType &ld,
                    const PointF& center,
                    const BoxF& micron_box,
                    double dbu,
                    int mode):
                    m_ld(ld),
                    m_center(center),
                    m_micron_box(micron_box),
                    m_found(false),
                    m_found_exact(false),
                    m_closest_distance(std::numeric_limits<double>::max()),
                    m_closest_distance_exact(std::numeric_limits<double>::max()),
                    m_dbu(dbu),
                    m_snap_mode(snap_mode_type(mode)){ }

    virtual void visit_instances(const std::vector<OasisElement*>& elements);
    virtual void visit_geometries(const std::vector<OasisElement*>& elements);
    virtual void visit_texts(const std::vector<OasisElement*>& elements) { }

    bool find_any() const
    {
        return m_found;
    }
    bool find_any_exact() const
    {
        return m_found_exact;
    }

    PointF get_closest_point() const
    {
        return m_closest_point;
    }

    PointF get_closest_point_exact() const
    {
        return m_closest_point_exact;
    }

    EdgeF get_closest_edge() const
    {
        return m_closest_edge;
    }

    EdgeF get_closest_edge_exact() const
    {
        return m_closest_edge_exact;
    }

    double get_closest_distance() const
    {
        return m_closest_distance;
    }

    double get_closest_distance_exact() const
    {
        return m_closest_distance_exact;
    }

private:
    LDType m_ld;
    PointF m_center;
    BoxF m_micron_box;

    bool m_found;
    bool m_found_exact;

    EdgeF m_closest_edge;
    EdgeF m_closest_edge_exact;

    PointF m_closest_point;
    PointF m_closest_point_exact;

    double m_closest_distance;
    double m_closest_distance_exact;

    double m_dbu;
    snap_mode_type m_snap_mode;

    void find_closest_point(Polygon& polygon);

    void check_edge(EdgeF& e);

    void search_closest(EdgeF& e);

    void compare_closest(const PointF& p, const EdgeF& e);

    void compare_closest_exact(const PointF& p, const EdgeF& e);

};

}

#endif // SNAPEDGEVISITOR_H
