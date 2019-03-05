#include "cell_visitor.h"
#include "oasis_layout.h"
#include "oasis_cell.h"

using namespace oasis;

void CellVisitor::visit_instances(const std::vector<OasisElement*>& elements)
{
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

void PrintVisitor::visit_geometries(const std::vector<OasisElement*>& elements)
{
    OAS_INFO << "geometry count " << elements.size() << OAS_ENDL;
    print_trans();
    print_qbox();
}

void PrintVisitor::visit_texts(const std::vector<OasisElement*>& elements)
{
    OAS_INFO << "text count " << elements.size() << OAS_ENDL;
}

void PrintVisitor::visit_instances(const std::vector<OasisElement*>& elements)
{
    OAS_INFO << "instance count " << elements.size() << OAS_ENDL;
    CellVisitor::visit_instances(elements);
}

void PrintVisitor::print_trans()
{
    const OasisTrans& trans = m_trans.top();
    OAS_INFO << "trans mirror " << trans.is_mirror()
        << " angle " << trans.angle()
        << " mag " << trans.mag()
        << " disp " << trans.disp().x() << "," << trans.disp().y()
        << OAS_ENDL;
}

void PrintVisitor::print_qbox()
{
    const Box& qbox = m_query_boxes.top();
    OAS_INFO << "qbox " << qbox.left() << "," << qbox.bottom() << "," << qbox.right() << "," << qbox.top() << OAS_ENDL;
}
