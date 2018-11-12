#include "render_layer_properties.h"

namespace render{

LayerProperties::LayerProperties()
    : m_frame_color(0),
      m_fill_color(0),
      m_pattern(0),
      m_line_style(0),
      m_transparent(false),
      m_visible(true),
      m_width(-1),
      m_metadata(),
      m_layer_index(1),
      mp_frame(0)
{

}

LayerProperties::LayerProperties(const LayerProperties& lp)
    : m_frame_color(0),
      m_fill_color(0),
      m_pattern(0),
      m_line_style(0),
      m_transparent(false),
      m_visible(true),
      m_width(-1),
      m_metadata(),
      m_layer_index(1),
      mp_frame(0)
{
    operator=(lp);
}

LayerProperties& LayerProperties::operator=(const LayerProperties& lp)
{
    if(this != &lp)
    {
        if(m_frame_color != lp.m_frame_color ||
           m_fill_color != lp.m_fill_color ||
           m_pattern != lp.m_pattern ||
           m_line_style != lp.m_line_style ||
           m_visible != lp.m_visible ||
           m_transparent != lp.m_transparent ||
           m_width != lp.m_width ||
           m_layer_index != lp.m_layer_index)
        {
            m_frame_color = lp.m_frame_color;
            m_fill_color = lp.m_fill_color;
            m_pattern = lp.m_pattern;
            m_line_style = lp.m_line_style;
            m_visible = lp.m_visible;
            m_transparent = lp.m_transparent;
            m_width = lp.m_width;
            m_layer_index = lp.m_layer_index;
        }
        if (m_metadata != lp.m_metadata)
        {
            m_metadata = lp.m_metadata;
        }

    }
    return *this;
}

bool LayerProperties::operator== (const LayerProperties & lp) const
{
    return m_frame_color == lp.m_frame_color &&
           m_fill_color == lp.m_fill_color &&
           m_pattern == lp.m_pattern &&
           m_line_style == lp.m_line_style &&
           m_visible == lp.m_visible &&
           m_transparent == lp.m_transparent &&
           m_width == lp.m_width &&
           m_layer_index == lp.m_layer_index &&
           m_metadata == lp.m_metadata;
}

void LayerProperties::set_metadata(const LayerMetaData &l_data)
{
    m_metadata = l_data;
}

void LayerProperties::set_metadata(const std::string &layer_name, int layer_num, int data_type)
{
    m_metadata.set_data_type(data_type);
    m_metadata.set_layer_name(layer_name);
    m_metadata.set_layer_num(layer_num);
}


}
