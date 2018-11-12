#ifndef RENDER_LAYER_METADATA_H
#define RENDER_LAYER_METADATA_H

#include <string>

namespace render{

class LayerMetaData
{
public:
    LayerMetaData();

    LayerMetaData(const std::string& layer_name, int layer_num, int data_type);

    LayerMetaData(const LayerMetaData& l);

    LayerMetaData& operator=(const LayerMetaData& l);

    bool operator==(const LayerMetaData &l) const;

    bool operator!=(const LayerMetaData & l) const
    {
        return !operator==(l);
    }

    const std::string& get_layer_name() const
    {
        return m_layer_name;
    }

    LayerMetaData& set_layer_name(const std::string& layer_name)
    {
        m_layer_name = layer_name;
        return *this;
    }

    int get_layer_num() const
    {
        return m_layer_num;
    }

    LayerMetaData& set_layer_num(int layer_num)
    {
        m_layer_num = layer_num;
        return *this;
    }

    LayerMetaData& set_data_type(int data_type)
    {
        m_data_type = data_type;
        return *this;
    }

    int get_data_type() const
    {
        return m_data_type;
    }

private:
    std::string m_layer_name;
    int m_layer_num;
    int m_data_type;
};

}

#endif // RENDER_LAYER_METADATA_H
