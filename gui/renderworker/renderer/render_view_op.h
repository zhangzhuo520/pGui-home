#ifndef RENDER_VIEWOP_H
#define RENDER_VIEWOP_H

#include <stdint.h>

namespace render{

typedef unsigned int color_t;
const unsigned int wordlen = 32;
const unsigned int wordbits = 6;
const unsigned int wordones = 0xffffffff;


class ViewOp
{
public:
    enum Mode{ Copy, Or, And, Xor};

    enum Shape{Rect, Cross};

    ViewOp();

    ViewOp(
        color_t color,
        Mode mode,
        unsigned int line_style_index,
        unsigned int pattern_index,
        Shape shape = Rect,
        int width = 1,
        int bitmap_index = -1);
    
    color_t ormask() const
    {
        return m_or;
    }

    color_t andmask() const
    {
        return m_and;
    }

    color_t xormask() const
    {
        return m_xor;
    }


    int width() const 
    {
        return m_width;
    }

    void width(int w)
    {
        m_width = w;
    }

    unsigned int line_style_index() const { return m_line_style_index; }

    void set_line_style_index(unsigned int i) { m_line_style_index = i; }

    unsigned int pattern_index() const { return m_pattern_index; }

    void  set_pattern_index(unsigned int i) { m_pattern_index = i; }
    
    Shape shape()  const { return m_shape; }

    void shape(Shape s) { m_shape = s; }

    void bitmap_index(int i) { m_bitmap_index = i; }

    int bitmap_index() const { return m_bitmap_index; }

    bool operator==(const ViewOp &v) const
    {
        return m_or == v.m_or &&
            m_and == v.m_and &&
            m_xor == v.m_xor &&
            m_pattern_index == v.m_pattern_index &&
            m_width == v.m_width &&
            m_shape == v.m_shape &&
            m_bitmap_index == v.m_bitmap_index;
    }
    
    bool operator< ( const ViewOp &v) const
    {
        if (m_bitmap_index != v.m_bitmap_index)
        {
            return m_bitmap_index < v.m_bitmap_index;
        }
        if (m_or !=  v.m_or)
        {
            return m_or < v.m_or;
        }
        if (m_and != v.m_and)
        {
            return m_and < v.m_and;
        }
        if (m_xor != v.m_xor)
        {
            return m_xor < v.m_xor;
        }
        if (m_pattern_index != v.m_pattern_index)
        {
            return m_pattern_index < v.m_pattern_index;
        }
        if (m_width != v.m_width)
        {
            return m_width < v.m_width;
        }
        if (m_bitmap_index != v.m_bitmap_index)
        {
            return m_bitmap_index < v.m_bitmap_index;
        }
    }

private:
    void init(color_t color, Mode mode);

private:
    color_t m_or;
    color_t m_and;
    color_t m_xor;
    unsigned int m_line_style_index;
    unsigned int m_pattern_index;
    Shape m_shape;
    int m_width;
    int m_bitmap_index;
};

}

#endif
