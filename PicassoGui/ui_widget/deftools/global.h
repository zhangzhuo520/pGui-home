#ifndef GLOBAL_H
#define GLOBAL_H
#include <QString>
#include <QColor>
#include <QVector>
#include <QBrush>
#include <QDateTime>
#include <QDebug>
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
        MarkCross
    };

    enum SnapFLag
    {
        SnapOpen,
        SnapClose
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
