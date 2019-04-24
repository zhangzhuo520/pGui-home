#ifndef SELECT_POLYGONS_VISITOR_H
#define SELECT_POLYGONS_VISITOR_H
#include "cell_visitor.h"

namespace oasis
{

class SelectPolygonsVisitor : public CellVisitor
{
public:
    SelectPolygonsVisitor(int64 select_index, LDType ld, std::vector<Polygon> &results) :
        m_select_index(select_index),
        m_ld(ld),
        m_results(results) { }

    virtual void visit_geometries(const std::vector<OasisElement*>& elements);
    virtual void visit_texts(const std::vector<OasisElement*>& elements) { }
    virtual void visit_instances(const std::vector<OasisElement*>& elements);

    void set_selected(bool selected) { m_selected = selected; }
    bool get_selected() const { return m_selected; }
private:
    int64 m_select_index;
    LDType m_ld;
    std::vector<Polygon>& m_results;
    bool m_selected;
};

}
#endif
