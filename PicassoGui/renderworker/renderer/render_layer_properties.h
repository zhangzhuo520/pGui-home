#ifndef RENDER_LAYER_PROPERTIES_H
#define RENDER_LAYER_PROPERTIES_H

#include "render_view_op.h"
#include "render_pattern.h"
#include "render_line_style.h"
#include "render_layer_metadata.h"

namespace render
{

class RenderFrame;

class LayerProperties
{
public:
    LayerProperties();

    LayerProperties(const LayerProperties& lp);

    LayerProperties& operator= (const LayerProperties& lp);

    bool operator== (const LayerProperties& lp) const;

    bool operator!= (const LayerProperties& lp) const
    {
        return !operator ==(lp);
    }

    color_t frame_color() const
    {
        return m_frame_color;
    }

    void set_frame_color(color_t color)
    {
        m_frame_color = color;
    }

    color_t fill_color() const
    {
        return m_fill_color;
    }

    void set_fill_color(color_t color)
    {
        m_fill_color = color;
    }

    int pattern() const
    {
        return m_pattern;
    }

    void set_pattern(int pattern)
    {
        m_pattern = pattern;
    }

    int line_style() const
    {
        return m_line_style;
    }

    void set_line_style(int line_style)
    {
        m_line_style = line_style;
    }

    bool transparent() const
    {
        return m_transparent;
    }

    void set_transparent(bool transparent)
    {
        m_transparent = transparent;
    }

    bool visible() const
    {
        return m_visible;
    }

    void set_visible(bool visible)
    {
        m_visible = visible;
    }

    int line_width() const
    {
        return m_line_width;
    }

    void set_line_width(int width)
    {
        m_line_width = width;
    }

    void set_metadata(const std::string& layer_name, int layer_num, int data_type);

    void set_metadata(const LayerMetaData & l_data);

    const LayerMetaData& metadata() const
    {
        return m_metadata;
    }

    int layer_index() const
    {
        return m_layer_index;
    }

    void set_layer_index(int layer_index)
    {
        m_layer_index = layer_index;
    }

    RenderFrame* view() const
    {
        return mp_frame;
    }

    void set_view(RenderFrame* view)
    {
        mp_frame = view;
    }

    int view_index() const
    {
        return m_layout_view_index;
    }

    void set_view_index(int index)
    {
        m_layout_view_index = index;
    }

private:
    mutable color_t m_frame_color;
    mutable color_t m_fill_color;
    mutable int m_pattern;
    mutable int m_line_style;
    bool m_transparent;
    bool m_visible;
    int m_line_width;

    render::LayerMetaData m_metadata;
    mutable int m_layer_index;

    RenderFrame* mp_frame;

    int m_layout_view_index;
};

}
#endif // RENDER_LAYER_PROPERTIES_H
