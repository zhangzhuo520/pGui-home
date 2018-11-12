#include "render_viewport.h"
#include <algorithm>

namespace render{

Viewport::Viewport():m_width(0), m_height(0)
{

}

Viewport::Viewport(unsigned int width, unsigned int height, const Oasis::OasisBox &target)
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

void Viewport::set_trans(const Oasis::OasisTrans& transform)
{
    m_trans = transform;
    Oasis::OasisPoint p1 = transform.trans(Oasis::OasisPoint(0, 0));
    Oasis::OasisPoint p2 = transform.trans(Oasis::OasisPoint(width(), height()));
    m_target_box = Oasis::OasisBox(p1.x(), p1.y(), p2.x(), p2.y());
}

void Viewport::set_box(const Oasis::OasisBox& box)
{
    m_target_box = box;

    Oasis::float64 box_width = box.right() - box.left();
    Oasis::float64 box_height = box.top() - box.bottom();
    Oasis::float64 x_ratio = box_width / (std::max(width(), (unsigned int) 1));
    Oasis::float64 y_ratio = box_height / (std::max(height(), (unsigned int) 1));

    Oasis::float64 ratio = std::max(x_ratio, y_ratio);

    if(ratio < 1e-13)
    {
        ratio = 0.001;
    }

    Oasis::float64 mx = Oasis::float64(box.right()) + Oasis::float64(box.left());
    Oasis::float64 my = Oasis::float64(box.top()) + Oasis::float64(box.bottom());

    Oasis::float64 dx = (Oasis::float64) (0.5 + ((mx) / ratio - Oasis::float64(width())) * 0.5);
    Oasis::float64 dy = (Oasis::float64) (0.5 + ((my) / ratio - Oasis::float64(height())) * 0.5);

    m_trans =  Oasis::OasisTrans(false, 0.0, (Oasis::float64)1.0 / ratio, Oasis::OasisPointF(-dx, -dy));
}

Oasis::OasisBox Viewport::box() const
{
    Oasis::OasisPoint p1 = trans().inverted().trans(Oasis::OasisPoint(0, 0));
    Oasis::OasisPoint p2 = trans().inverted().trans(Oasis::OasisPoint(width(), height()));
    return Oasis::OasisBox(p1.x(), p1.y(), p2.x(), p2.y());

}

}
