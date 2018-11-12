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
    
signals:
    
public slots:

protected:
    void paintEvent(QPaintEvent *);

    void resizeEvent(QResizeEvent *);

    void mouseMoveEvent(QMouseEvent *);

private:
    int ScaleUnit;

    QImage *image;

    QPixmap cursorPixmap;

    int imageH;

    int imageW;

    int maxLimit;

    int hTextNumber;

    int wTextNumber;

    QHBoxLayout *Hlayout;
};
}
#endif // SCALEFRAME_H
