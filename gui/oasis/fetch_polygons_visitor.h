#ifndef FETCH_POLYGONS_VISITOR_H
#define FETCH_POLYGONS_VISITOR_H
#include "cell_visitor.h"

class PolygonKey;

namespace oasis
{

class FetchPolygonsVisitor : public CellVisitor
{
public:
    FetchPolygonsVisitor(
        LDType ld,
        std::vector<Polygon> *results,
        std::vector<PolygonKey>* attach_keys) :
        m_ld(ld),
        m_results(results),
        m_attach_keys(attach_keys) { }

    virtual void visit_geometries(const std::vector<OasisElement*>& elements);
    virtual void visit_texts(const std::vector<OasisElement*>& elements) { }

private:
    LDType m_ld;
    std::vector<Polygon>* m_results;
    std::vector<PolygonKey>* m_attach_keys;
};

}
#endif
