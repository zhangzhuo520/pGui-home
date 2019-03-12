#ifndef GLOBAL_H
#define GLOBAL_H
#include <QString>
#include <QColor>
#include <QVector>
#include <QBrush>
#include <QDateTime>
#include <QFile>
#include <QDebug>
#include <QApplication>

#include "../../qt_logger/qtlogger.h"
#include "../../qt_logger/pgui_log_global.h"
namespace ui {

class Global
{
public:
    enum PaintStyle
    {
        Normal,
        Mark,
        Measrue
    };

    enum PaintTool
    {
        Nothing,
        MeasureLine,
        MeasureAngle,
        MarkCross,
        RemoveLine
    };

    enum SnapFLag
    {
        SnapOpen,
        SnapClose
    };

    enum AngleDirection
    {
        Any = 2,
        Horizontal,
        Vertical,
        Pri_Diagonal, // 135
        Aux_Dignonal  //45
    };

//    static int screen_width;

//    static int screen_height;
};

class UiStyle{
public:
    static QColor ToolBarColor;
    static QColor ButtonPressColor;
    static QColor DockTitleBarColor;
    static QColor ButtonHoverColor;
    static QVector <QColor> ItemColorList;
    static QVector <int> ItemPetternList;


};
}
#endif // GLOBAL_H
