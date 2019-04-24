#include "fetch_polygons_visitor.h"
#include "oasis_cell.h"
#include "layout_utilities.h"

using namespace oasis;

static bool check_ld(int32 layer, int32 datatype, const LDType& ld)
{
    if (ld.layer == layer && ld.datatype == datatype)
    {
        return true;
    }
    return false;
}

void FetchPolygonsVisitor::visit_geometries(const std::vector<OasisElement*>& elements)
{
    Box& qbox = m_query_boxes.top();
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
            m_results->push_back(p);
            if (m_attach_keys)
            {
                m_attach_keys->push_back(PolygonKey(m_info_index, trans));
            }
        }
    }
}

