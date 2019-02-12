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

    bool read_yaml(const QString&);

    void nodePrint(const YAML::Node&);

private:
    QStringList m_layername_list;
};
}
#endif // YAMLPARSING_H
