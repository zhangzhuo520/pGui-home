#ifndef RENDER_DEFECT_POINT_H
#define RENDER_DEFECT_POINT_H

#include "render_view_object.h"

namespace render{

class DefectPoint: public RenderObject
{
public:

    DefectPoint(double x, double y, RenderObjectWidget* widget = 0, bool is_static = true);

    virtual void render(const render::Viewport& vp, RenderObjectWidget* frame);

    QColor frame_color() const
    {
        return m_frame_color;
    }

    QColor fill_color() const
    {
        return m_fill_color;
    }

    int line_style() const
    {
        return m_line_style;
    }
    int pattern() const
    {
        return m_pattern;
    }

    int line_width() const
    {
        return m_line_width;
    }

    void set_line_style(int line_style);

    void set_pattern(int pattern);

    void set_frame_color(QColor frame_color);

    void set_fill_color(QColor fill_color);

    double x() const
    {
        return m_x;
    }

    double y() const
    {
        return m_y;
    }

    void set_point(double x, double y);

private:

    QColor m_frame_color;
    QColor m_fill_color;
    int m_line_style;
    int m_pattern;
    int m_line_width;

    double m_x;
    double m_y;
};

}
#endif // RENDER_DEFECT_POINT_H
