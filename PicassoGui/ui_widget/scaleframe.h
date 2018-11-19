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
#include "drawwidget.h"
namespace UI {
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

    void updataAxisDate();
    
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
