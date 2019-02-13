#ifndef YAMLPARSING_H
#define YAMLPARSING_H

#ifndef Q_MOC_RUN
#include "yaml-cpp/yaml.h"
#include "yaml-cpp/parser.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <QDebug>
#endif
#include <QStringList>
#include <QString>
namespace ui
{
class YamlParsing
{
public:
    YamlParsing();

    void read_yaml(const QString&);

    void read_layername(const YAML::Node&);

    QStringList get_layername_list();

private:
    QStringList m_layername_list;
};
}
#endif // YAMLPARSING_H
