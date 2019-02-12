#ifndef GLOBAL_H
#define GLOBAL_H
#include <QString>
#include <QColor>
#include <QVector>
#include <QBrush>
#include <QDateTime>
#include <QFile>
#include <QDebug>
#include "../../qt_logger/qtlogger.h"
#include "../../qt_logger/pgui_log_global.h"
namespace ui {

#ifndef TEST_DEBUG
#define TEST_DEBUG  qDebug()<<"[FILE:"<<__FILE__<<",LINE"<<__LINE__<<",FUNC"<<__FUNCTION__<<"]";
#endif

#ifndef TIME_DEBUG
#define TIME_DEBUG qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd") +" "+QDateTime::currentDateTime().toString("hh:mm:ss.zzz") + "ms"<<":"\
    <<"[" << "FILE:"<<__FILE__<<",LINE("<<__LINE__<<"),FUNC" <<__FUNCTION__<< "]";
#endif


#ifndef NeedContinue
#define NeedContinue  qDebug()<<"[FILE:"<<__FILE__<<",LINE"<<__LINE__<<",FUNC"<<__FUNCTION__<<"] Donn't finished!";
#endif

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
