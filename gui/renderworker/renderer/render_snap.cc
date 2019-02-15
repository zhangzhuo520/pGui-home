#include "render_snap.h"
#include "render_frame.h"
namespace render
{

class SnapSearcher
{
public:

    SnapSearcher(oasis::PointF p1):m_p1(p1),m_found(false)
    {

    }

    void find(RenderFrame* frame, int snap_mode, oasis::float64 snap_range)
    {
        if(!frame)
        {
            return;
        }

        m_box.set_left(m_p1.x() - snap_range);
        m_box.set_bottom(m_p1.y() - snap_range);
        m_box.set_right(m_p1.x() + snap_range);
        m_box.set_top(m_p1.y() + snap_range);
        double distance = std::numeric_limits<double>::max();
        for(size_t i = 0; i < frame->layers_size(); i ++)
        {
            const LayerProperties* lp = frame->get_properties(i);
            if(lp->visible())
            {
                LayoutView* lv = frame->get_layout_view(lp->view_index());
                oasis::OasisLayout* layout = lv->get_layout();
                oasis::float64 dbu = layout->get_dbu();
                oasis::Box box(rint(m_box.left() / dbu),
                                    rint(m_box.bottom() / dbu),
                                    rint(m_box.right() / dbu),
                                    rint(m_box.top() / dbu));
                oasis::LDType ld(lp->metadata().get_layer_num(), lp->metadata().get_data_type());
                layout->snapping_edge(-1, box, oasis::OasisTrans(), ld, m_p1, m_box, &m_snap_p, &m_snap_edge, &distance, &m_found, snap_mode);
            }
        }
    }

    oasis::EdgeF get_snapped_edge()
    {
        return m_snap_edge;
    }

    oasis::PointF get_snapped_point()
    {
        return m_snap_p;
    }

    bool is_found()
    {
        return m_found;
    }

public:
    oasis::EdgeF m_snap_edge;
    oasis::PointF m_snap_p;
    oasis::PointF m_p1;
    oasis::BoxF m_box;
    bool m_found;
};

class PolygonSearcher
{
public:
    PolygonSearcher(oasis::Point p1): m_p1(p1), m_priority(-1)
    {

    }


    void find(RenderFrame* frame, oasis::float64 snap_range)
    {
        if(!frame)
        {
            return;
        }

        oasis::OasisTrans trans = frame->get_trans();
        oasis::Point p1 = trans.inverted().trans(oasis::Point(0, 0));
        oasis::Point p2 = trans.inverted().trans(oasis::Point(snap_range, 0));
        oasis::int64 box_width = (oasis::int64) std::max(fabs(p2.x() - p1.x()), fabs(p2.y() - p1.y()));
        oasis::Box box(m_p1.x() - box_width, m_p1.y() - box_width, m_p1.x() + box_width, m_p1.y() + box_width);
        for(size_t i = 0; i < frame->layers_size(); i ++)
        {
            const LayerProperties* lp = frame->get_properties(i);
            if(lp->visible())
            {
                std::vector<oasis::Polygon> results;
                LayoutView* lv = frame->get_layout_view(lp->view_index());
                oasis::OasisLayout* layout = lv->get_layout();
                oasis::LDType ld(lp->metadata().get_layer_num(), lp->metadata().get_data_type());
                layout->get_polygons(-1, box, oasis::OasisTrans(), ld, results);
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

    oasis::Polygon get_polygon()
    {
        return m_polygon;
    }

private:
    bool m_found;
    oasis::Point m_p1;
    oasis::Polygon m_polygon;
    int m_priority;

};

std::pair<bool,oasis::PointF> snap_point(RenderFrame *frame, oasis::PointF p1, oasis::float64 snap_range, int mode)
{

    SnapSearcher searcher(p1);
    searcher.find(frame, mode, snap_range);
    if(searcher.is_found())
    {
        return std::make_pair(true, searcher.get_snapped_point());
    }
    else{
        return std::make_pair(false, p1);
    }

}

//std::pair<bool, oasis::EdgeF> snap_edge(RenderFrame *frame, oasis::PointF p1, oasis::float64 snap_range)
//{

//    SnapSearcher searcher(p1);
//    searcher.find(frame, snap_range);
//    if(searcher.is_found())
//    {
//        return std::make_pair(true, searcher.get_snapped_edge());
//    }
//    else{
//        return std::make_pair(false, oasis::EdgeF());
//    }

//}

//std::pair<bool, oasis::Polygon> collect_polygon(RenderFrame *frame, oasis::Point p1, oasis::float64 snap_range)
//{
//    PolygonSearcher searcher(p1);
//    searcher.find(frame, snap_range);
//    if(searcher.is_found())
//    {
//        return std::make_pair(true, searcher.get_polygon());
//    }
//    else{
//        return std::make_pair(false, oasis::Polygon());
//    }

//}

}

