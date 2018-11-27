#include "scaleframe.h"
#include <QPushButton>
namespace UI
{

ScaleFrame::ScaleFrame(QWidget *parent) :
    QFrame(parent),
		xStart(0),
	xEnd(0),
	yStart(0),
	yEnd(0),
    imageW(parent->width()),
    imageH(parent->height())
{

    Hlayout = new QHBoxLayout(this);
    Hlayout->setContentsMargins(xSpace, ySpace, 0, 0);
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
    painter.drawLine(QPoint(18, 18), QPoint(imageW, 18));
    painter.drawLine(QPoint(18, 18), QPoint(18, imageH));

    darw_X_axis(painter);
    darw_Y_axis(painter);

    cursorPixmap = QPixmap::fromImage(*image);
}

double ScaleFrame::toDouble_1(double x)
{
   QString str = QString::number(x, 'g', 6);
   QStringList list = str.split('.');
   double pointNum = list.at(1).left(1).toDouble() * 0.1;
   double value = 0;
   if (x >= 0)
   {
       value  = list.at(0).toDouble() + pointNum;
   }
   else
   {
       value  = list.at(0).toDouble() - pointNum;
   }

   return value;
}

void ScaleFrame::darw_X_axis(QPainter &painter)
{
    double k = (imageW - xSpace) / (xEnd - xStart);
    double pointX = 0;
    double zoomIn = xEnd - xStart;

    if (zoomIn >= 1600)
    {
        for (int i = 0; i < zoomIn;)
        {
            int tempX = 0;
            if (xStart > 0)
            {
                tempX = (xStart + 1000) / 1000;
                tempX = tempX * 1000;
            }
            else
            {
                tempX = xStart / 1000;
                tempX = tempX * 1000;
            }
            double tempDouble = tempX - xStart;
            pointX = xSpace + (tempDouble + i) * k;

            if (pointX >= xSpace)
            {
                painter.drawLine(QPoint(pointX, axis_unit_start), QPoint(pointX, axis_unit_long));
                painter.drawText(QPoint(pointX + 2, axis_unit_short - 2), QString::number(tempX + i, 'f', 0));
            }

            if (0 == i)
            {
                for(double m = 0; m < tempDouble; m ++)
                {
                    int minX = m * k * 100;
                    if ((pointX - minX) >= xSpace)
                    {
                        painter.drawLine(QPoint(pointX - minX, axis_unit_start), QPoint(pointX - minX, axis_unit_short));
                    }
                }
            }

            for(int j = 0; j < 10; j ++)
            {
                double minX = pointX + j * k * 100;

                if (fabs(xEnd - minX) > UI::esp)
                {
                    if (minX >= xSpace)
                    {
                        painter.drawLine(QPoint(minX, axis_unit_start), QPoint(minX, axis_unit_short));
                    }
                }
            }
            i = i + 1000;
        }
    }
    else if (zoomIn > 200 && zoomIn < 1600)
    {
        for (int i = 0; i < zoomIn;)
        {
            int tempX = 0;
            if (xStart > 0)
            {
                tempX = (xStart + 100) / 100;
                tempX = tempX * 100;
            }
            else
            {
                tempX = xStart / 100;
                tempX = tempX * 100;
            }
            double tempDouble = tempX - xStart;
            pointX = xSpace + (tempDouble + i) * k;

            if (pointX >= xSpace)
            {
                painter.drawLine(QPoint(pointX, axis_unit_start), QPoint(pointX, axis_unit_long));
                painter.drawText(QPoint(pointX + 2, axis_unit_short - 2), QString::number(tempX + i, 'f', 0));
            }

            if (0 == i)
            {
                for(double m = 0; m < tempDouble; m ++)
                {
                    int minX = m * k * 10;
                    if ((pointX - minX) >= xSpace)
                    {
                        painter.drawLine(QPoint(pointX - minX, axis_unit_start), QPoint(pointX - minX, axis_unit_short));
                    }
                }
            }

            for(int j = 0; j < 10; j ++)
            {
                double minX = pointX + j * k * 10;

                if (fabs(xEnd - minX) > UI::esp)
                {
                    if (minX >= xSpace)
                    {
                        painter.drawLine(QPoint(minX, axis_unit_start), QPoint(minX, axis_unit_short));
                    }
                }
            }
            i = i + 100;
        }
    }
    else if (zoomIn > 30 && zoomIn < 200)
    {
        for (int i = 0; i < zoomIn;)
        {
            int tempX = 0;
            if (xStart > 0)
            {
                tempX = (xStart + 10) / 10;
                tempX = tempX * 10;
            }
            else
            {
                tempX = xStart / 10;
                tempX = tempX * 10;
            }

            double tempDouble = tempX - xStart;
            pointX = xSpace + (tempDouble + i) * k;

            if (pointX >= xSpace)
            {
                painter.drawLine(QPoint(pointX, axis_unit_start), QPoint(pointX, axis_unit_long));
                painter.drawText(QPoint(pointX + 2, axis_unit_short - 2), QString::number(tempX + i, 'f', 0));
            }

            if (0 == i)
            {
                for(double m = 0; m < tempDouble; m ++)
                {
                    int minX = m * k;
                    if ((pointX - minX) >= xSpace)
                    {
                        painter.drawLine(QPoint(pointX - minX, axis_unit_start), QPoint(pointX - minX, axis_unit_short));
                    }
                }
            }

            for(int j = 0; j < 10; j ++)
            {
                double minX = pointX + j * k;

                if (fabs(xEnd - minX) > UI::esp)
                {
                    if (minX >= xSpace)
                    {
                        painter.drawLine(QPoint(minX, axis_unit_start), QPoint(minX, axis_unit_short));
                    }
                }
            }
            i = i + 10;
        }
    }
    else if (zoomIn < 30 && zoomIn > 2)
    {
        for (int i = 0;  i < zoomIn; i ++)
        {
            int tempX = 0;
            double tempDouble = 0;
            int textNum = 0;
            if(xStart >= 0)
            {
                tempX = xStart;
                tempDouble = xStart - tempX;
                tempDouble = 1 - tempDouble;
                if (tempX == xStart)
                {
                    textNum = xStart;
                }
                else
                {
                    textNum = xStart + 1;
                }
            }
            else
            {
                tempX = xStart;
                tempDouble = tempX - xStart;
                textNum = xStart;
            }

            pointX = xSpace + (tempDouble + i) * k;

            if (pointX >= xSpace)
            {
                painter.drawLine(QPoint(pointX, axis_unit_start), QPoint(pointX, axis_unit_long));
                painter.drawText(QPoint(pointX + 2, axis_unit_short - 3), QString::number(textNum + i));
            }

            if (0 == i)
            {
                for(double m = 0; m < tempDouble;)
                {
                    double minX = m * k;
                    if ((pointX - minX) >= xSpace)
                    {
                        painter.drawLine(QPoint(pointX - minX, axis_unit_start), QPoint(pointX - minX, axis_unit_short));
                    }
                    m = m + 0.1;
                }
            }

            for(int j = 0; j < 10; j ++)
            {
                double minX = pointX + (0.1 * j) * k;

                if (fabs(xEnd - minX) > UI::esp)
                {
                    if (minX >= xSpace)
                    {
                        painter.drawLine(QPoint(minX, axis_unit_start), QPoint(minX, axis_unit_short));
                    }
                }
            }
        }
    }
    else if(zoomIn <= 2)
    {
        for (double i = 0; i < zoomIn;)
        {
            double tempX = 0;
            double tempDouble = 0;
            if(xStart >= 0)
            {
                tempX = toDouble_1(xStart);
                tempDouble = tempX - xStart + 0.1;
                if (tempX == xStart)
                {
                    ;
                }
                else
                {
                    tempX = tempX + 0.1;
                }
            }
            else
            {
                tempX = toDouble_1(xStart);
                tempDouble = tempX - xStart;          
            }

            pointX = xSpace + (tempDouble + i) * k;


            if (pointX >= xSpace)
            {
                painter.drawLine(QPointF(pointX, axis_unit_start), QPointF(pointX, axis_unit_long));
                painter.drawText(QPointF(pointX + 2, axis_unit_short - 2), QString::number(tempX + i, 'f', 1));
            }

            if (0 == i)
            {
                for(double m = 0; m < tempDouble;)
                {
                    double minX = m * k;
                    if ((pointX - minX) >= 20)
                    {
                        painter.drawLine(QPointF(pointX - minX, axis_unit_start), QPointF(pointX - minX, axis_unit_short));
                    }
                    m = m + 0.01;
                }
            }

            for(int j = 0; j < 10; j ++)
            {
                double minX = pointX + (0.01 * j) * k;

                if (fabs(xEnd - minX) > UI::esp)
                {
                    if (minX >= xSpace)
                    {
                        painter.drawLine(QPointF(minX, axis_unit_start), QPointF(minX, axis_unit_short));
                    }
                }
            }
            i = i + 0.1;
        }
    }
}

void ScaleFrame::darw_Y_axis(QPainter & painter)
{
    double k = (imageH - 20) / (yEnd - yStart);
    double pointY = 0;
    double zoomIn = yEnd - yStart;

    if (zoomIn >= 1600)
    {
        for (int i = 0; i < (yEnd - yStart);)
        {
            int tempY = 0;
			double tempDouble = 0;
            if (yEnd > 0)
            {
                tempY = yEnd / 1000;
                tempY = tempY * 1000;
				tempDouble = yEnd - tempY;
            }
            else
            {
                tempY = (yEnd - 1000) / 1000;
                tempY = tempY * 1000;
				tempDouble = - (tempY - yEnd);
            }

            pointY = ySpace + (tempDouble + i) * k;
            
            if (pointY >= ySpace)
            {
                painter.drawLine(QPoint(axis_unit_start, pointY), QPoint(axis_unit_long, pointY));
                painter.resetTransform();
                painter.translate(5 , pointY + 2);   //set Rotate Center
                painter.rotate(90);
                painter.drawText(QPoint(0, 0), QString::number(tempY - i , 'f', 0));
                painter.rotate(-90);
                painter.resetTransform();
            }

            if (0 == i)
            {
                for(double m = 0; m < tempDouble; )
                {
                    int miny = m * k;
                    if ((pointY - miny) >= 20)
                    {
                        painter.drawLine(QPoint(axis_unit_start, pointY - miny), QPoint(axis_unit_short, pointY - miny));
                    }
					m = m + 100;
                }
            }

            for(int j = 0; j < 10; j ++)
            {
                double minX = pointY + j * k * 100;

                if (fabs(xStart - minX) > UI::esp)
                {
                    if (minX >= 20)
                    {
                        painter.drawLine(QPoint(axis_unit_start, minX), QPoint(axis_unit_short, minX));
                    }
                }
            }
            i = i + 1000;
        }
    }
    else if (zoomIn > 200 && zoomIn < 1600)
    {
        for (int i = 0; i < (yEnd - yStart);)
        {
            int tempY = 0;
			double tempDouble = 0;
            if (yEnd > 0)
            {
                tempY = yEnd / 100;
                tempY = tempY * 100;
				tempDouble = yEnd - tempY;
            }
            else
            {
                tempY = (yEnd - 100) / 100;
                tempY = tempY * 100;
				tempDouble = - (tempY - yEnd);
            }

            pointY = ySpace + (tempDouble + i) * k;
            
            if (pointY >= ySpace)
            {
                painter.drawLine(QPoint(axis_unit_start, pointY), QPoint(axis_unit_long, pointY));
                painter.resetTransform();
                painter.translate(5 , pointY + 2);   //set Rotate Center
                painter.rotate(90);
                painter.drawText(QPoint(0, 0), QString::number(tempY - i , 'f', 0));
                painter.rotate(-90);
                painter.resetTransform();
            }

            if (0 == i)
            {
                for(double m = 0; m < tempDouble; )
                {
                    int miny = m * k;
                    if ((pointY - miny) >= ySpace)
                    {
                        painter.drawLine(QPoint(axis_unit_start, pointY - miny), QPoint(axis_unit_short, pointY - miny));
                    }
					m = m + 10;
                }
            }

            for(int j = 0; j < 10; j ++)
            {
                double minX = pointY + j * k * 10;

                if (fabs(xStart - minX) > UI::esp)
                {
                    if (minX >= ySpace)
                    {
                        painter.drawLine(QPoint(axis_unit_start, minX), QPoint(axis_unit_short, minX));
                    }
                }
            }
            i = i + 100;
        }
    }
    else if (zoomIn > 30 && zoomIn < 200)
    {
         for (int i = 0; i < (yEnd - yStart);)
        {
            int tempY = 0;
			double tempDouble = 0;
            if (yEnd > 0)
            {
                tempY = yEnd / 10;
                tempY = tempY * 10;
				tempDouble = yEnd - tempY;
            }
            else
            {
                tempY = (yEnd - 10) / 10;
                tempY = tempY * 10;
				tempDouble = - (tempY - yEnd);
            }

            pointY = ySpace + (tempDouble + i) * k;
            
            if (pointY >= ySpace)
            {
                painter.drawLine(QPoint(axis_unit_start, pointY), QPoint(axis_unit_long, pointY));
                painter.resetTransform();
                painter.translate(5 , pointY + 2);   //set Rotate Center
                painter.rotate(90);
                painter.drawText(QPoint(0, 0), QString::number(tempY - i , 'f', 0));
                painter.rotate(-90);
                painter.resetTransform();
            }

            if (0 == i)
            {
                for(double m = 0; m < tempDouble; )
                {
                    int miny = m * k;
                    if ((pointY - miny) >= 20)
                    {
                        painter.drawLine(QPoint(axis_unit_start, pointY - miny), QPoint(axis_unit_short, pointY - miny));
                    }
					m = m + 1;
                }
            }

            for(int j = 0; j < 10; j ++)
            {
                double minX = pointY + j * k;

                if (fabs(xStart - minX) > UI::esp)
                {
                    if (minX >= 20)
                    {
                        painter.drawLine(QPoint(axis_unit_start, minX), QPoint(axis_unit_short, minX));
                    }
                }
            }
            i = i + 10;
        }
    }
    else if (zoomIn < 30 && zoomIn > 2)
    {
           for (int i = 0; i < (yEnd - yStart);)
        {
            int tempY = 0;
			double tempDouble = 0;
            if (yEnd > 0)
            {
                tempY = yEnd;
				tempDouble = yEnd - tempY;
            }
            else
            {
                tempY = yEnd;
				tempDouble = - (tempY - yEnd);
            }

            pointY = ySpace + (tempDouble + i) * k;
            
            if (pointY >= ySpace)
            {
                painter.drawLine(QPoint(axis_unit_start, pointY), QPoint(axis_unit_long, pointY));
                painter.resetTransform();
                painter.translate(5 , pointY + 2);   //set Rotate Center
                painter.rotate(90);
                painter.drawText(QPoint(0, 0), QString::number(tempY - i , 'f', 0));
                painter.rotate(-90);
                painter.resetTransform();
            }

            if (0 == i)
            {
                for(double m = 0; m < tempDouble; )
                {
                    int miny = m * k;
                    if ((pointY - miny) >= ySpace)
                    {
                        painter.drawLine(QPoint(axis_unit_start, pointY - miny), QPoint(axis_unit_short, pointY - miny));
                    }
					m = m + 0.1;
                }
            }

            for(int j = 0; j < 10; j ++)
            {
                double minX = pointY + j * k * 0.1;

                if (fabs(xStart - minX) > UI::esp)
                {
                    if (minX >= ySpace)
                    {
                        painter.drawLine(QPoint(axis_unit_start, minX), QPoint(axis_unit_short, minX));
                    }
                }
            }
            i = i + 1;
        }
    }
    if(zoomIn <= 2)
    {
		
        for (double i = 0; i < (yEnd - yStart);)
        {
            double tempY = 0;
			double tempDouble = 0;
            if (yEnd > 0)
            {
                tempY = toDouble_1(yEnd);
				tempDouble = yEnd - tempY;
            }
            else
            {
                tempY = toDouble_1(yEnd);
				if(tempY == yEnd)
				{
					;
				}
				else
				{
					tempY = tempY + 0.1;
				}
				
				tempDouble = - (tempY - yEnd);
            }

            pointY = ySpace + (tempDouble + i) * k;
            
            if (pointY >= ySpace)
            {
                painter.drawLine(QPoint(axis_unit_start, pointY), QPoint(axis_unit_long, pointY));
                painter.resetTransform();
                painter.translate(5 , pointY + 2);   //set Rotate Center
                painter.rotate(90);
                painter.drawText(QPoint(0, 0), QString::number(tempY - i , 'f', 1));
                painter.rotate(-90);
                painter.resetTransform();
            }

            if (0 == i)
            {
                for(double m = 0; m < tempDouble; )
                {
                    int miny = m * k;
                    if ((pointY - miny) >= ySpace)
                    {
                        painter.drawLine(QPoint(axis_unit_start, pointY - miny), QPoint(axis_unit_short, pointY - miny));
                    }
					m = m + 0.01;
                }
            }

			
            for(int j = 0; j < 10; j ++)
            {
                double minX = pointY + j * k * 0.01;

                if (fabs(xStart - minX) > UI::esp)
                {
                    if (minX >= ySpace)
                    {
                        painter.drawLine(QPoint(axis_unit_start, minX), QPoint(axis_unit_short, minX));
                    }
                }
            }
            i = i + 0.1;
        }
    }
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


void ScaleFrame::slot_box_updated(double left, double bot, double right, double top)
{
    xStart = left;
    xEnd = right;
    yStart = bot;
    yEnd = top;
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

