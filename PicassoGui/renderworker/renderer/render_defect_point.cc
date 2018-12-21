#include "render_defect_point.h"
#include "render_bitmap.h"
#include "render_viewport.h"

namespace render{

DefectPoint::DefectPoint(double x, double y, RenderObjectWidget *widget, bool is_static)
    :RenderObject(widget, is_static),
     m_frame_color(QColor(255, 0, 0)),
     m_fill_color(QColor(255, 0, 0)),
     m_line_style(0),
     m_pattern(0),
     m_line_width(1),
     m_x(x),
     m_y(y)
{

}

void DefectPoint::set_line_style(int line_style)
{
    if(m_line_style != line_style)
    {
        m_line_style = line_style;
        redraw();
    }
}

void  DefectPoint::set_pattern(int pattern)
{
    if(m_pattern != pattern)
    {
        m_pattern = pattern;
        redraw();
    }
}

void DefectPoint::set_frame_color(QColor frame_color)
{
    if(m_frame_color != frame_color)
    {
        m_frame_color = frame_color;
        redraw();
    }
}

void DefectPoint::set_fill_color(QColor fill_color)
{
    if(m_fill_color != fill_color)
    {
        m_fill_color = fill_color;
        redraw();
    }
}

void DefectPoint::set_point(double x, double y)
{
    if(m_x != x || m_y != y)
    {
        m_x = x;
        m_y = y;
        redraw();
    }
}


void DefectPoint::render(const Viewport &vp, RenderObjectWidget *widget)
{
    Oasis::OasisTrans m_trans = vp.trans();
    Oasis::OasisPointF defect_point(m_x, m_y);
    Oasis::OasisPointF center = defect_point;
    Oasis::OasisPointF point = m_trans.trans(center);

    render::RenderEdge left_edge(point.x() - 3, point.y() -3, point.x() - 3, point.y() + 3);
    render::RenderEdge top_edge(point.x() - 3, point.y() + 3, point.x() + 3, point.y() + 3);
    render::RenderEdge right_edge(point.x() + 3, point.y() + 3, point.x() + 3, point.y() - 3);
    render::RenderEdge bot_edge(point.x() + 3, point.y() - 3, point.x() - 3, point.y() - 3);

    std::vector<render::RenderEdge> edges;
    edges.reserve(4);
    edges.push_back(left_edge);
    edges.push_back(top_edge);
    edges.push_back(right_edge);
    edges.push_back(bot_edge);

    std::vector<render::ViewOp> result;
    result.reserve(3);
    render::Bitmap* frame, *fill;

    if(m_pattern >= 0)
    {
        fill = widget->planes(render::ViewOp(m_fill_color.rgb(),
                                             render::ViewOp::Copy,
                                             0,
                                             (unsigned int) m_pattern,
                                             render::ViewOp::Rect,
                                             m_line_width));
        fill->render_fill_ortho(edges);
    }
    else{
        fill = 0;
    }

    frame = widget->planes(render::ViewOp(m_frame_color.rgb(),
                                          render::ViewOp::Copy,
                                          (unsigned int) m_line_style,
                                          0,
                                          render::ViewOp::Rect,
                                          m_line_width));
    frame->render_contour_ortho(edges);

}

}
