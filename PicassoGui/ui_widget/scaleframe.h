#ifndef SCALEFRAME_H
#define SCALEFRAME_H

#include <QFrame>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QPoint>
#include <QLayout>
#include <QMouseEvent>
#include <QDebug>
#include <math.h>
#include "drawwidget.h"
namespace UI {

const double esp = 0.000001;
const int xSpace = 18;
const int ySpace = 18;
const int axis_unit_start = 18;
const int axis_unit_short = 15;
const int axis_unit_long = 5;


class ScaleFrame : public QFrame
{
    Q_OBJECT
public:
    explicit ScaleFrame(QWidget *parent = 0);

    void initImage();

    void paintImage();

    void setImageSize(QSize);

    void setDrawWidget(QWidget *);

    void updateMouseCursor(QPoint);

    double toDouble_1(double);

    void darw_X_axis(QPainter &);

    void darw_Y_axis(QPainter &);
signals:
    
public slots:
    void slot_box_updated(double,double,double,double);

protected:
    void paintEvent(QPaintEvent *);

    void resizeEvent(QResizeEvent *);

    void mouseMoveEvent(QMouseEvent *);

private:
    int ScaleUnit;

    QImage *image;

    QPixmap cursorPixmap;

    double xStart;

    double xEnd;

    double yStart;

    double yEnd;

    int imageW;

    int imageH;

    QHBoxLayout *Hlayout;
};
}
#endif // SCALEFRAME_H
