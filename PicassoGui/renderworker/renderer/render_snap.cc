#include "render_snap.h"
#include "render_frame.h"

namespace render
{

class SnapSearcher
{
public:
    SnapSearcher(Oasis::OasisPoint p1):m_p1(p1),m_found(false)
    {

    }

    void find(RenderFrame* frame, int snap_range)
    {
        if(!frame)
        {
            return;
        }

        Oasis::OasisTrans trans = frame->get_trans();
        Oasis::OasisPoint p1 = trans.inverted().trans(Oasis::OasisPoint(0, 0));
        Oasis::OasisPoint p2 = trans.inverted().trans(Oasis::OasisPoint(snap_range, 0));
        Oasis::int64 box_width = (Oasis::int64) std::max(fabs(p2.x() - p1.x()), fabs(p2.y() - p1.y()));
        Oasis::OasisBox box(m_p1.x() - box_width, m_p1.y() - box_width, m_p1.x() + box_width, m_p1.y() + box_width);

        double distance = std::numeric_limits<double>::max();
        for(size_t i = 0; i < frame->layers_size(); i ++)
        {
            const LayerProperties* lp = frame->get_properties(i);
            if(lp->visible())
            {
                LayoutView lv = frame->get_layout_view(lp->view_index());
                Oasis::OasisLayout* layout = lv.get_layout();
                Oasis::LDType ld(lp->metadata().get_layer_num(), lp->metadata().get_data_type());
                layout->SnapEdge(-1, box, Oasis::OasisTrans(), ld, m_p1, &m_snap_p, &m_snap_edge, &distance, &m_found);
            }
        }
    }

    Oasis::OasisEdge get_snapped_edge()
    {
        return m_snap_edge;
    }

    Oasis::OasisPoint get_snapped_point()
    {
        return m_snap_p;
    }

    bool is_found()
    {
        return m_found;
    }

public:
    Oasis::OasisEdge m_snap_edge;
    Oasis::OasisPoint m_snap_p;
    Oasis::OasisPoint m_p1;
    bool m_found;
};

class PolygonSearcher
{
public:
    PolygonSearcher(Oasis::OasisPoint p1): m_p1(p1), m_priority(-1)
    {

    }


    void find(RenderFrame* frame, int snap_range)
    {
        if(!frame)
        {
            return;
        }

        Oasis::OasisTrans trans = frame->get_trans();
        Oasis::OasisPoint p1 = trans.inverted().trans(Oasis::OasisPoint(0, 0));
        Oasis::OasisPoint p2 = trans.inverted().trans(Oasis::OasisPoint(snap_range, 0));
        Oasis::int64 box_width = (Oasis::int64) std::max(fabs(p2.x() - p1.x()), fabs(p2.y() - p1.y()));
        Oasis::OasisBox box(m_p1.x() - box_width, m_p1.y() - box_width, m_p1.x() + box_width, m_p1.y() + box_width);

        for(size_t i = 0; i < frame->layers_size(); i ++)
        {
            const LayerProperties* lp = frame->get_properties(i);
            if(lp->visible())
            {
                std::vector<Oasis::OasisPolygon> results;
                LayoutView lv = frame->get_layout_view(lp->view_index());
                Oasis::OasisLayout* layout = lv.get_layout();
                Oasis::LDType ld(lp->metadata().get_layer_num(), lp->metadata().get_data_type());
                layout->GetPolygons(-1, box, Oasis::OasisTrans(), ld, results);
                if(!results.empty())
                {
                    if((int)i > m_priority)
                    {
                        m_priority = i;
                        m_polygon = results[0];
                    }

                }

            }
        }
    }

    bool is_found() const
    {
        return m_found;
    }

    Oasis::OasisPolygon get_polygon()
    {
        return m_polygon;
    }

private:
    bool m_found;
    Oasis::OasisPoint m_p1;
    Oasis::OasisPolygon m_polygon;
    int m_priority;

};

std::pair<bool,Oasis::OasisPoint> snap_point(RenderFrame *frame, Oasis::OasisPoint p1, int snap_range)
{

    SnapSearcher searcher(p1);
    searcher.find(frame, snap_range);
    if(searcher.is_found())
    {
        return std::make_pair(true, searcher.get_snapped_point());
    }
    else{
        return std::make_pair(false, p1);
    }

}

std::pair<bool, Oasis::OasisEdge> snap_edge(RenderFrame *frame, Oasis::OasisPoint p1, int snap_range)
{

    SnapSearcher searcher(p1);
    searcher.find(frame, snap_range);
    if(searcher.is_found())
    {
        return std::make_pair(true, searcher.get_snapped_edge());
    }
    else{
        return std::make_pair(false, Oasis::OasisEdge());
    }

}

std::pair<bool, Oasis::OasisPolygon> collect_polygon(RenderFrame *frame, Oasis::OasisPoint p1, int snap_range)
{
    PolygonSearcher searcher(p1);
    searcher.find(frame, snap_range);
    if(searcher.is_found())
    {
        return std::make_pair(true, searcher.get_polygon());
    }
    else{
        return std::make_pair(false, Oasis::OasisPolygon());
    }

}

}

