#ifndef CELL_VISITOR_H
#define CELL_VISITOR_H
#include "oasis_types.h"
#include <stack>

namespace oasis
{

class OasisLayout;
class OasisCell;
class OasisElement;
class CellVisitor
{
public:
    const Box& query_box() const { return m_query_boxes.top(); }
    void new_query_box(const Box& box) { m_query_boxes.top().intersect(box); }
    void set_top_query_box(const Box& box) { m_query_boxes.push(box); }
    void set_top_trans(const OasisTrans &trans) { m_trans.push(trans); }
    void set_layout(OasisLayout* layout) { m_layout = layout; }
    void set_info_index(int64 index) { m_info_index = index; }
    OasisLayout* get_layout() { return m_layout; }

    virtual void visit_geometries(const std::vector<OasisElement*>& elements) = 0;
    virtual void visit_texts(const std::vector<OasisElement*>& elements) = 0;
    virtual void visit_instances(const std::vector<OasisElement*>& elements);
protected:
    OasisLayout* m_layout;
    int64 m_info_index;
    std::stack<Box> m_query_boxes;
    std::stack<OasisTrans> m_trans;
};

class PrintVisitor : public CellVisitor
{
public:
    virtual void visit_geometries(const std::vector<OasisElement*>& elements);
    virtual void visit_texts(const std::vector<OasisElement*>& elements);
    virtual void visit_instances(const std::vector<OasisElement*>& elements);
private:
    void print_trans();
    void print_qbox();
};

}
#endif
