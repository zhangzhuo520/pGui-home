#include "render_viewport.h"
#include <algorithm>

namespace render{

Viewport::Viewport():m_width(0), m_height(0)
{

}

Viewport::Viewport(unsigned int width, unsigned int height, const oasis::BoxF &target)
{
    m_width = width;
    m_height = height;
    set_box(target);
}

void Viewport::set_size(unsigned int width, unsigned int height)
{
    m_width = width;
    m_height = height;
    set_box(m_target_box);
}

void Viewport::set_trans(const oasis::OasisTrans& transform)
{
    m_trans = transform;
    oasis::PointF p1 = transform.transF(oasis::PointF(0, 0));
    oasis::PointF p2 = transform.transF(oasis::PointF(width(), height()));
    m_target_box = oasis::BoxF(p1.x(), p1.y(), p2.x(), p2.y());
}

void Viewport::set_box(const oasis::BoxF& box)
{
    m_target_box = box;

    oasis::float64 box_width = box.right() - box.left();
    oasis::float64 box_height = box.top() - box.bottom();
    oasis::float64 x_ratio = box_width / (std::max(width(), (unsigned int) 1));
    oasis::float64 y_ratio = box_height / (std::max(height(), (unsigned int) 1));

    oasis::float64 ratio = std::max(x_ratio, y_ratio);

    if(ratio < 1e-13)
    {
        ratio = 0.001;
    }

    oasis::float64 mx = oasis::float64(box.right()) + oasis::float64(box.left());
    oasis::float64 my = oasis::float64(box.top()) + oasis::float64(box.bottom());

    oasis::float64 dx = floor (0.5 + (mx / ratio - oasis::float64(width())) * 0.5);
    oasis::float64 dy = floor (0.5 + (my / ratio - oasis::float64(height())) * 0.5);

    m_trans =  oasis::OasisTrans(false, 0.0, (oasis::float64)1.0 / ratio, oasis::PointF(-dx, -dy));
}

oasis::BoxF Viewport::box() const
{
    oasis::PointF p1 = trans().inverted().transF(oasis::PointF(0, 0));
    oasis::PointF p2 = trans().inverted().transF(oasis::PointF(width(), height()));
    return oasis::BoxF(p1.x(), p1.y(), p2.x(), p2.y());

}

}
