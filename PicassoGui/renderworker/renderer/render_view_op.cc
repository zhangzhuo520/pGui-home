#include "render_view_op.h"

namespace render{

ViewOp::ViewOp() :
    m_line_style_index(0),
    m_pattern_index(0),
    m_shape(Rect),
    m_width(0), //line_width
    m_bitmap_index(0)
{
    init(0, Copy);
}

ViewOp::ViewOp(
    color_t color,
    Mode mode,
    unsigned int line_style_index,
    unsigned int pattern_index,
    Shape shape,
    int width,
    int bitmap_index) :
    m_line_style_index(line_style_index),
    m_pattern_index(pattern_index),
    m_shape(shape),
    m_width(width),
    m_bitmap_index(bitmap_index)
{
    init(color, mode);
}


void ViewOp::init(color_t color, Mode mode)
{
    m_or = (mode == Copy || mode == Or) ? color : 0;
    m_and = (mode == Copy || mode == And) ? color : wordones;
    m_xor = (mode == Xor) ? color : 0;
}


}
