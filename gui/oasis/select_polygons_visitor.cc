#include "select_polygons_visitor.h"
#include "oasis_cell.h"
#include "oasis_layout.h"

using namespace oasis;

static bool check_ld(int32 layer, int32 datatype, const LDType& ld)
{
    if (ld.layer == layer && ld.datatype == datatype)
    {
        return true;
    }
    return false;
}

void SelectPolygonsVisitor::visit_geometries(const std::vector<OasisElement*>& elements)
{
    if (!get_selected())
    {
        return;
    }

    Box &qbox = m_query_boxes.top();
    OasisTrans& trans = m_trans.top();
    for(uint32 i = 0; i < elements.size(); ++i)
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
        for(uint32 k = 0; k < disps.size(); ++k)
        {
            Box b = box;
            b.shift(disps[k]);
            OAS_DBG << " b " << b.left() << "," << b.bottom() << "," << b.right() << "," << b.top() << OAS_ENDL;
            if(!qbox.overlap(b))
            {
                continue;
            }

            Polygon p = polygon.shifted(disps[k]);
            p.trans(trans);
            m_results.push_back(p);
        }
    }
}

void SelectPolygonsVisitor::visit_instances(const std::vector<OasisElement*>& elements)
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

            bool old_selected = get_selected();
            set_selected(old_selected || p->get_info_index() == m_select_index);
            cell.accept_visitor(*this);
            set_selected(old_selected);

            m_query_boxes.pop();
            m_trans.pop();
        }
    }
}
