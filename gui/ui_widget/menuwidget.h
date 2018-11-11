#ifndef MENUWIDGET_H
#define MENUWIDGET_H

#include <QObject>
#include <QMenu>
#include <QLayout>
#include <QColorDialog>
#include <QLabel>
#include <QPalette>
#include <QWidgetAction>
#include <QWidget>
#include <QDebug>
#include <QMouseEvent>
#include <QEvent>
#include <QPushButton>
#include <QVector>
#include "deftools/defcontrols.h"
#include <QSpacerItem>
#include <QColorDialog>
#include <QPainter>
#include <QPen>
#include <QPaintEvent>
#include <QBrush>

class StyleItem :public QLabel
{
    Q_OBJECT
public:
    StyleItem(QWidget *parent, QBrush Brush)
    {
        Q_UNUSED(parent);
        brush = Brush;
        setAutoFillBackground(true);
        setStyle(brush);

    }

    ~StyleItem()
    {

    }

    void setStyle(QBrush Brush)
    {
        QPalette palette = this->palette();
        palette.setBrush(QPalette::Background, Brush);
        setPalette(palette);
    }
public slots:
    void slot_selectItemStyle();

protected:
    void mousePressEvent(QMouseEvent *e)
    {
        Q_UNUSED(e);
        qDebug() << brush;
    }

    void paintEvent(QPaintEvent *e)
    {
        Q_UNUSED(e);
        QPainter painter(this);
        painter.setPen(QPen(Qt::black));
        painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
    }

    void enterEvent(QEvent *e)
    {
        Q_UNUSED(e)

//        QColor selectColor(10, 10, 200, 100);
//        setColor(selectColor);
    }

    void leaveEvent(QEvent *e)
    {
        Q_UNUSED(e)
//        setColor(color);
    }
private:
    QBrush brush;
};

class ColorItem : public QLabel
{
    Q_OBJECT
public:
    ColorItem(QWidget *parent, QColor Color)
    {
        Q_UNUSED(parent);
        color = Color;
        setAutoFillBackground(true);
        setColor(color);

    }

    ~ColorItem()
    {

    }

    void setColor(QColor color)
    {
        QPalette palette = this->palette();
        palette.setColor(QPalette::Background, color);
        setPalette(palette);
    }
public slots:
    void slot_selectItemColor();

protected:
    void mousePressEvent(QMouseEvent *e)
    {
        Q_UNUSED(e);
        qDebug() << color;
    }

    void paintEvent(QPaintEvent *e)
    {
        Q_UNUSED(e);
        QPainter painter(this);
        painter.setPen(QPen(Qt::black));
        painter.drawRect(QRect(0, 0, width(), height()));
    }

    void enterEvent(QEvent *e)
    {
        Q_UNUSED(e)

        QColor selectColor(10, 10, 200, 100);
        setColor(selectColor);
    }

    void leaveEvent(QEvent *e)
    {
        Q_UNUSED(e)
        setColor(color);
    }

private:
    QColor color;
};

class StyleWidget: public QWidget
{
public:
    StyleWidget(QWidget *parent = 0);

    void initBrushList();

    void initStyleHistory();

    void initStyleFrame();

private:
    QFrame *StyleFrame;

    StyleItem* Item;

    QBrush *brush;

    QHBoxLayout *Hlayout;

    QPalette pale;

    QVector <QBrush> BrushList;
};

class ColorWidget: public QWidget
{
    Q_OBJECT
public:
    ColorWidget(QWidget *parent = 0);

    void initColorList();

    void initColorFrame();

    void initColorHistory();

public slots:
    void slot_selectColor();

private:
    PushButton *ColorButton;

    QVBoxLayout * Vlayout;

    QWidget *VlayoutWidget;

    QHBoxLayout *Hlayout;

    QFrame *ColorFrame;

    ColorItem *cItem;

    QPalette pale;

    QVector <QColor> ColorList;

    QVector <ColorItem *> cItemList;
};

class MenuColor : public QMenu
{
    Q_OBJECT
public:
    explicit MenuColor(QString objectName);
    ~MenuColor(){}


signals:


private:
    QWidgetAction *ActionWidget;

    ColorWidget *colorwidget;
};

class MenuStyle : public QMenu
{
    Q_OBJECT
public:
    explicit MenuStyle(QString objectName);

private:
    StyleWidget *stylewidget;

    QWidgetAction *ActionWidget;
};

#endif // MENUWIDGET_H
