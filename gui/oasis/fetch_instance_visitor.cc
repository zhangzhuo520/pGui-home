#include "fetch_instance_visitor.h"
#include "oasis_layout.h"
#include "oasis_cell.h"

using namespace oasis;

void FetchInstanceVisitor::visit_instances(const std::vector<OasisElement*>& elements)
{
    const OasisTrans& trans = m_trans.top();

    if (elements.empty())
    {
        if (m_leaves.count(m_info_index))
        {
            m_instances[m_info_index].push_back(trans);
        }
        return;
    }

    CellVisitor::visit_instances(elements);
}

