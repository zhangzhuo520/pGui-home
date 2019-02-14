#include "yamlparsing.h"
namespace ui
{
YamlParsing::YamlParsing()
{
}

void YamlParsing::read_yaml(const QString & yamlPath)
{
    YAML::Node config = YAML::LoadFile(yamlPath.toStdString());
    YAML::Node mask = config["mask"];
    YAML::Node layers = mask["layers"];
    for (unsigned int i = 0; i < layers.size(); i++)
    {
        read_layername(layers[i]);
    }
}

void YamlParsing::read_layername(const YAML::Node & node)
{
    m_layername_list.append(QString::fromStdString(node["name"].as<std::string>()));
}

QStringList YamlParsing::get_layername_list()
{
    return m_layername_list;
}
}
