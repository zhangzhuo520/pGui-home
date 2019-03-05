#include "count_mark_visitor.h"
#include "oasis_layout.h"
#include "oasis_cell.h"

using namespace oasis;

void CountMarkVisitor::visit_instances(const std::vector<OasisElement*>& elements)
{
    for (uint32 i = 0; i < elements.size(); ++i)
    {
        PlacementElement* p = dynamic_cast<PlacementElement*>(elements[i]);
        oas_assert(p != NULL);
        std::vector<Point> disps;
        p->get_disps(disps);
        OasisCellInfo& cellinfo = m_layout->get_cell_info(p->get_info_index());
        OasisCell& cell = m_layout->get_cell(cellinfo.get_cell_index());
        for(uint32 k = 0; k < disps.size(); ++k)
        {
            if (cellinfo.get_name() == m_subname)
            {
                ++m_count;
                m_parent_to_subname[m_parent_name] = cellinfo.get_name();
                continue;
            }
            bool found = false;
            for (auto it = m_corner_to_count.begin(); it != m_corner_to_count.end(); ++it)
            {
                std::string subname = m_subname + "_" + it->first;
                if (cellinfo.get_name() == subname)
                {
                    ++m_count;
                    ++m_corner_to_count[it->first];
                    found = true;
                    break;
                }
            }
            if (found)
            {
                continue;
            }

            std::string old_parent = get_parent_name();
            set_parent_name(cellinfo.get_name());
            cell.accept_visitor(*this);
            set_parent_name(old_parent);
        }
    }
}

