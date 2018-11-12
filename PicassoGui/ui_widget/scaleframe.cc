#include "scaleframe.h"
#include <QPushButton>
namespace UI
{

ScaleFrame::ScaleFrame(QWidget *parent) :
    QFrame(parent),
    ScaleUnit(16),
    maxLimit(200),
    wTextNumber(16),
    hTextNumber(16),
    imageH(parent->height()),
    imageW(parent->width())
{

    Hlayout = new QHBoxLayout(this);
    Hlayout->setContentsMargins(20, 23, 0, 0);
    setMouseTracking(true);
    initImage();
    paintImage();
    update();
}

void ScaleFrame::initImage()
{
    image = new QImage(imageW,imageH,QImage::Format_RGB32);
    QColor backColor = qRgb(230, 230, 230);
    image->fill(backColor);
}

void ScaleFrame::paintImage()
{
    QPainter painter(image);
    QPen penDegree;
    penDegree.setColor(qRgb(0, 0, 0));
    painter.setPen(penDegree);
    painter.drawLine(QPoint(18, 18), QPoint(imageW, 18));
    painter.drawLine(QPoint(18, 18), QPoint(18, imageH));

    int pointx = 20;
    int pointy = 20;     // set startPoint of the axis
    int width = imageW-pointx;
    int height = imageH;   //set ScaleRuler Height and width

    painter.drawLine(pointx, pointy - 2, pointx, pointy - 15);
    painter.drawText(pointx + 3, pointy - 11, "0");

    painter.drawLine(pointy - 2, pointx, pointy - 15, pointx);
    painter.resetTransform();
    painter.translate(pointx - 11 , pointy + 8);   //set Rotate Center
    painter.rotate(-90);
    painter.drawText(0 , 0, "0");      //drawText in new RatateCenter
    painter.rotate(90);
    painter.resetTransform();
    for(int i = 0; i < ScaleUnit; i ++)
    {
        int bigUintLength = pointx + (i + 1) * width / 16;

        painter.drawLine(bigUintLength, pointy - 2, bigUintLength, pointy - 15);
        for (int j = 0; j < 10; j ++)
        {
            int smallUnitLength = pointx + (j + i * 10 + 1) * width / 160;
            if (j == 4)
            {
                painter.drawLine(smallUnitLength, pointy - 2, smallUnitLength, pointy - 12);
            }
            else
            {
                painter.drawLine(smallUnitLength, pointy - 2, smallUnitLength, pointy - 8);
            }
        }
        painter.drawText(pointx + (i + 1.1) * width / 16, pointy - 11, QString::number((int)((i + 1) * ((double)wTextNumber / 16))));
    }

    for(int i = 0; i < ScaleUnit; i ++)
    {
        int bigUintLength = pointy + (i + 1) * height / 16;

        painter.drawLine(pointy - 15, bigUintLength, pointy - 2, bigUintLength);
        for (int j = 0; j < 10; j ++)
        {
            int smallUnitLength = pointy + (j + i * 10 + 1) * height / 160;
            if (j == 4)
            {
                painter.drawLine(pointy - 12, smallUnitLength, pointy - 2, smallUnitLength);
            }
            else
            {
                painter.drawLine(pointy - 8, smallUnitLength, pointy - 2, smallUnitLength);
            }
        }

        //setRotate(-90d) Text
        painter.resetTransform();
        painter.translate(pointx - 19, pointy + (i + 1.05) * height / 16);
        painter.rotate(90);
        painter.drawText(0, 0, QString::number((int)((i + 1) * ((double)wTextNumber / 16))));
        painter.rotate(-90);
        painter.resetTransform();
    }
    cursorPixmap = QPixmap::fromImage(*image);
}

void ScaleFrame::setImageSize(QSize size)
{
    imageW = size.width();
    imageH = size.height();
}

void ScaleFrame::setDrawWidget(QWidget *widget)
{
    Hlayout->addWidget(widget);
}

void ScaleFrame::updateMouseCursor(QPoint pos)
{
    cursorPixmap = QPixmap::fromImage(*image);
    QPainter painter(&cursorPixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(QBrush(Qt::black));

    QPolygon topPolygon;
    topPolygon.append(QPoint(pos.x() - 5, 14));
    topPolygon.append(QPoint(pos.x() + 5, 14));
    topPolygon.append(QPoint(pos.x(), 18));

    QPolygon leftPolygon;
    leftPolygon.append(QPoint(14, pos.y() - 5));
    leftPolygon.append(QPoint(14, pos.y() + 5));
    leftPolygon.append(QPoint(18, pos.y()));

    painter.drawPolygon(topPolygon);
    painter.drawPolygon(leftPolygon);
}

void ScaleFrame::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.drawPixmap(0, 0, cursorPixmap);
}

void ScaleFrame::resizeEvent(QResizeEvent *e)
{
    setImageSize(e->size());
    initImage();
    paintImage();
    update();
}

void ScaleFrame::mouseMoveEvent(QMouseEvent *e)
{
    qDebug() << "2222222222222222" << e->type();
    updateMouseCursor(e->pos());
    update();
}
}

