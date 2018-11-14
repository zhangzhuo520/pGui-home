#include "render_layer_metadata.h"

namespace render{

LayerMetaData::LayerMetaData()
{
    m_layer_name = "";
    m_layer_num = -1;
    m_data_type = -1;
}

LayerMetaData::LayerMetaData(const std::string &layer_name, int layer_num, int data_type)
    :m_layer_name(layer_name),
     m_layer_num(layer_num),
     m_data_type(data_type)
{

}

LayerMetaData::LayerMetaData(const LayerMetaData &l)
{
    operator=(l);
}

LayerMetaData& LayerMetaData::operator =(const LayerMetaData &l)
{
    if(this != &l)
    {
        m_layer_name = l.m_layer_name;
        m_layer_num = l.m_layer_num;
        m_data_type = l.m_data_type;
    }
    return *this;
}

bool LayerMetaData::operator ==(const LayerMetaData &l) const
{
    return m_layer_name == l.m_layer_name && m_layer_num == l.m_layer_num && m_data_type == l.m_data_type;
}

}
