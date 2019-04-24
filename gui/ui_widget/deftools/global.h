#ifndef GLOBAL_H
#define GLOBAL_H
#include <QString>
#include <QColor>
#include <QVector>
#include <QBrush>
#include <QDateTime>
#include <QFile>
#include <QDebug>
//#include <QApplication>
//#include <QCoreApplication>

#include "../../qt_logger/qtlogger.h"
#include "../../qt_logger/pgui_log_global.h"
namespace ui {
class QApplication;
class QDesktopWidget;
class Global
{
public:
    enum PaintStyle
    {
        Normal = 0,
        Mark,
        Measrue,
        RtsCutLine
    };

    enum PaintTool
    {
        Nothing = 0,
        MeasureLine,
        MeasureAngle,
        MarkCross,
        RemoveLine,
        CutLine,
        MeasureOblique
    };

    enum SnapFLag
    {
        SnapOpen = 0,
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
    
    enum RtsCutLineAngle
    {
        HVD = 0,
        HV,
        Any_Angle,
        User_Input_coord,
        User_Input_Angle
    };


    static bool is_new_dbformat;

    static bool delet_dir(QString);

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
