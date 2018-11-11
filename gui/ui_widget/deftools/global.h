#ifndef GLOBAL_H
#define GLOBAL_H
#include <QString>
#include <QColor>
#include <QVector>
#include <QBrush>

class Global
{
public:
    enum PaintStyle
    {
        MouseStyle,
        PenStyle,
        RulerStyle
    };
};

class UiStyle{
public:
    static QColor ToolBarColor;
    static QColor ButtonPressColor;
    static QColor ButtonHoverColor;
    static QString TitleColor;
    static QString ButtonStyle;
    static QString MenuBarColor;
    static QString StateBarColor;
    static QString DialogBorderColor;
    static QVector <QColor> ItemColorList;
    static QVector <QBrush> ItemBrushList;
    static QString DockWidgetStyle;
    static QString CommboxStyle;
    static QString TabWidgetStyle;
    static QString TitleStyle;
    static QString MenuBarStyle;
    static QString ToolbarStyle;
    static QString MainWindowStyle;
    static QColor DockTitleBarColor;
    static QString MenuStyle;
    static QString StatusBarStyle;
    static QString ActionStyle;
};

#endif // GLOBAL_H
