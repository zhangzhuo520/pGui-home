#ifndef FETCH_INSTANCE_VISITOR_H
#define FETCH_INSTANCE_VISITOR_H
#include "cell_visitor.h"
#include <set>
#include <map>
#include <vector>

namespace oasis
{

class FetchInstanceVisitor : public CellVisitor
{
public:
    FetchInstanceVisitor(const std::set<int32>& leaves, std::map<int32, std::vector<OasisTrans> >& instances) : m_leaves(leaves), m_instances(instances) {}
    virtual void visit_geometries(const std::vector<OasisElement*>& elements) {}
    virtual void visit_texts(const std::vector<OasisElement*>& elements) {}
    virtual void visit_instances(const std::vector<OasisElement*>& elements);

private:
    const std::set<int32>& m_leaves;
    std::map<int32, std::vector<OasisTrans> >& m_instances;
};

}
#endif
