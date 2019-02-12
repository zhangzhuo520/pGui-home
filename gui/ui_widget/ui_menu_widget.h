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
#include <QSpacerItem>
#include <QColorDialog>
#include <QPainter>
#include <QPen>
#include <QPaintEvent>
#include <QBrush>
#include <QBitmap>
#include <QImage>

#include "deftools/defcontrols.h"
#include "../../renderworker/renderer/render_pattern.h"


namespace ui {
class StyleItem :public QLabel
{
    Q_OBJECT
public:
    StyleItem(QWidget *parent, int patternId)
    {
        Q_UNUSED(parent);
        id = patternId;
        image = pattern.get_bitmap(patternId, 24 ,16).toImage().convertToFormat(QImage::Format_RGB32);
    }

    ~StyleItem()
    {}

    QColor uint_to_color(uint color);

    QImage &image_color_change(QImage &, QColor);
signals:
    void signal_selectItemStyle(int);

protected:
    void mousePressEvent(QMouseEvent *e)
    {
        Q_UNUSED(e);
        signal_selectItemStyle(id);
    }

    void paintEvent(QPaintEvent *e)
    {
        Q_UNUSED(e);
        QPainter painter(this);
        painter.drawImage(0, 0, image);
        painter.setPen(QPen(Qt::black));
        if (id == 3)
        {
            painter.drawRect(QRect(0, 0, 23, 15));
        }
        else
        {
            painter.drawRect(QRect(0, 0, 22, 15));
        }
    }

    void enterEvent(QEvent *e)
    {
        Q_UNUSED(e)
        image_color_change(image, Qt::yellow);
        update();
    }

    void leaveEvent(QEvent *e)
    {
        Q_UNUSED(e)
        image_color_change(image, Qt::white);
        update();
    }
private:
    QBitmap bitmap;

    QImage image;

    render::Pattern pattern;

    int id;
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

    void setColor(QColor Color)
    {
        color = Color;
        QPalette palette = this->palette();
        palette.setColor(QPalette::Background, color);
        setPalette(palette);
    }

protected:
    void mousePressEvent(QMouseEvent *e)
    {
        Q_UNUSED(e);
        emit signal_select_color(color);
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
        QPalette palette = this->palette();
        palette.setColor(QPalette::Background, selectColor);
        setPalette(palette);
    }

    void leaveEvent(QEvent *e)
    {
        Q_UNUSED(e)
        setColor(color);
    }
signals:
    void signal_select_color(QColor);
private:
    QColor color;
};

class StyleWidget: public QWidget
{
    Q_OBJECT
public:
    StyleWidget(QWidget *parent = 0);

    void initPatternList();

    void initStyleHistory();

    void initStyleFrame();
public slots:
    void slot_selectItemStyle(int);

signals:
    void signal_selectItemStyle(int);

private:
    QFrame *StyleFrame;

    StyleItem* Item;

    QBrush *brush;

    QHBoxLayout *Hlayout;

    QPalette pale;

    QVector <int> PatternList;

    render::Pattern pattern;
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
    void slot_selectColor(QColor);

    void slot_colorDialogSelect();

signals:
    void signal_selectColor(QColor);

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
public slots:
    void slot_seletColor(QColor);

signals:
    void signal_selectColor(QColor);

private:
    QWidgetAction *ActionWidget;

    ColorWidget *colorwidget;
};

class MenuStyle : public QMenu
{
    Q_OBJECT
public:
    explicit MenuStyle(QString objectName);

public slots:
    void slot_selectItemStyle(int);

signals:
    void signal_selectItemStyle(int);

private:
    StyleWidget *stylewidget;

    QWidgetAction *ActionWidget;
};
}
#endif // MENUWIDGET_H
