#include "scaleframe.h"
#include <QPushButton>
namespace UI
{

ScaleFrame::ScaleFrame(QWidget *parent) :
    QFrame(parent),
    ScaleUnit(16),
    xStart(0),
    xEnd(16),
    yStart(0),
    yEnd(16),
    imageW(parent->width()),
    imageH(parent->height())
{

    Hlayout = new QHBoxLayout(this);
    Hlayout->setContentsMargins(20, 20, 0, 0);
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
    image->fill(Qt::white);
    QPainter painter(image);
    QPen penDegree;
    penDegree.setColor(qRgb(0, 0, 0));
    painter.setPen(penDegree);
    painter.drawLine(QPoint(20, 20), QPoint(imageW, 20));
    painter.drawLine(QPoint(20, 20), QPoint(20, imageH));

    int pointx = 20;
    int pointy = 22;     // set startPoint of the axis
    int width = imageW-pointx;
    int height = imageH;   //set ScaleRuler Height and width

    painter.drawLine(pointx, pointy - 2, pointx, pointy - 15);
    painter.drawText(pointx + 3, pointy - 11, QString::number(xStart, 'g', 2));

    painter.drawLine(pointy - 2, pointx, pointy - 13, pointx);
    painter.resetTransform();
    painter.translate(pointx - 8 , pointy + 6);   //set Rotate Center
    painter.rotate(-90);
    painter.drawText(0 , 0, QString::number(yStart, 'g', 2));      //drawText in new RatateCenter
    painter.rotate(90);
    painter.resetTransform();
    for(int i = 0; i < ScaleUnit; i ++)
    {
        int bigUintLength = pointx + (i + 1) * width / ScaleUnit;

        painter.drawLine(bigUintLength, pointy - 2, bigUintLength, pointy - 15);
        for (int j = 0; j < 10; j ++)
        {
            int smallUnitLength = pointx + (j + i * 10 + 1) * width / ScaleUnit * 10;
            if (j == 4)
            {
                painter.drawLine(smallUnitLength, pointy - 2, smallUnitLength, pointy - 12);
            }
            else
            {
                painter.drawLine(smallUnitLength, pointy - 2, smallUnitLength, pointy - 8);
            }
        }
        painter.drawText(pointx + (i + 1.1) * width / ScaleUnit, pointy - 11, QString::number((xStart + (1 + i) * (xEnd - xStart) / ScaleUnit), 'g', 2));
    }

    for(int i = 0; i < ScaleUnit; i ++)
    {
        int bigUintLength = pointy + (i + 1) * height / ScaleUnit;

        painter.drawLine(pointy - 15, bigUintLength, pointy - 2, bigUintLength);
        for (int j = 0; j < 10; j ++)
        {
            int smallUnitLength = pointy + (j + i * 10 + 1) * height / ScaleUnit * 10;
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
        painter.translate(pointx - 19, pointy + (i + 1.05) * height / ScaleUnit);
        painter.rotate(90);
        painter.drawText(0, 0, QString::number((yStart + (1 + i) * (yEnd - yStart) / ScaleUnit), 'g', 2));
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

    double cursorX = pos.x();
    double cursorY = pos.y();

    if (cursorX < 20)
    {
        cursorX = 20;
    }

    if (cursorY < 20)
    {
        cursorY = 20;
    }

    QPolygon topPolygon;
    topPolygon.append(QPoint(cursorX - 5, 14));
    topPolygon.append(QPoint(cursorX + 5, 14));
    topPolygon.append(QPoint(cursorX, 18));

    QPolygon leftPolygon;
    leftPolygon.append(QPoint(14, cursorY - 5));
    leftPolygon.append(QPoint(14, cursorY + 5));
    leftPolygon.append(QPoint(18, cursorY));

    painter.drawPolygon(topPolygon);
    painter.drawPolygon(leftPolygon);
}

void ScaleFrame::updataAxisDate()
{
}

void ScaleFrame::slot_box_updated(double left, double bot, double right, double top)
{
    qDebug() << left << bot << right << top;
    xStart = left;
    xEnd = right;
    yStart = top;
    yEnd = bot;
    paintImage();
    update();
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
    updateMouseCursor(e->pos());
    update();
}
}

