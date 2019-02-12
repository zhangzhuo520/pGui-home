#include "yamlparsing.h"
namespace ui
{
YamlParsing::YamlParsing()
{
}

bool YamlParsing::read_yaml(const QString & yamlPath)
{
    YAML::Node config = YAML::LoadFile(yamlPath.toStdString());
    YAML::Node mask = config["mask"];
    YAML::Node layers = mask["layers"];
    qDebug() << config.size();
    for (unsigned int i = 0; i < config.size(); i++)
    {
        nodePrint(layers[i]);
    }
}

void YamlParsing::nodePrint(const YAML::Node & node)
{
    int id = node["in_corner"].as<int>();
    m_layername_list.append(QString::fromStdString(node["name"].as<std::string>()));
    for(int i = 0; i < m_layername_list.size(); i ++)
    qDebug() << m_layername_list.at(i);
    qDebug() << id;
}
}
