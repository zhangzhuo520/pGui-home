#include "ui_scale_frame.h"
#include <QPushButton>
namespace ui
{

ScaleFrame::ScaleFrame(QWidget *parent) :
    QFrame(parent),
    m_xstart(0),
    m_xend(0),
    m_ystart(0),
    m_yend(0),
    m_image_width(parent->width()),
    m_image_heigth(parent->height()),
    m_current_posX(0),
    m_current_posY(0)
{
    m_hlayout = new QHBoxLayout(this);
    m_hlayout->setContentsMargins(0, 0, 0, 0);
    setMouseTracking(true);
    initRenderFrame();
    initImage();
    paintImage();
    emit signal_box_updated();
}

void ScaleFrame::initRenderFrame()
{
    m_render_frame = new render::RenderFrame(this);
    m_draw_widget = new DrawWidget(m_render_frame);
    m_render_frame->set_cursor_widget(m_draw_widget);
    m_hlayout->addWidget(m_render_frame);
    connect(m_render_frame, SIGNAL(signal_box_updated(double,double,double,double)), this, SLOT(slot_box_updated(double,double,double,double)));
    connect(this, SIGNAL(signal_box_updated()), m_render_frame, SLOT(slot_box_updated()));
    connect(m_render_frame, SIGNAL(signal_pos_updated(double,double)), this, SLOT(slot_pos_updated(double, double)));
    connect(m_draw_widget, SIGNAL(signal_distancePoint(QPointF,QPointF)), m_render_frame, SLOT(slot_distance_point(QPointF,QPointF)));
    connect(m_draw_widget, SIGNAL(signal_updataDistance(double)), this, SLOT(slot_distance_updated(double)));
    connect(m_draw_widget, SIGNAL(signal_moveCenter()), this, SLOT(slot_move_point_center()));
}

void ScaleFrame::slot_move_point_center()
{
    m_render_frame->center_at_point(m_current_posX, m_current_posY);
}

void ScaleFrame::set_defect_point(double x, double y)
{
    m_render_frame->set_defect_point(x, y);
}

void ScaleFrame::initImage()
{
    m_image = new QImage(m_image_width,m_image_heigth,QImage::Format_RGB32);
    QColor backColor = qRgb(230, 230, 230);
    m_image->fill(backColor);
}

void ScaleFrame::paintImage()
{
    m_image->fill(Qt::white);
    QPainter painter(m_image);
    QPen penDegree;
    penDegree.setColor(qRgb(0, 0, 0));
    painter.setPen(penDegree);
    painter.drawLine(QPoint(18, 18), QPoint(m_image_width, 18));
    painter.drawLine(QPoint(18, 18), QPoint(18, m_image_heigth));

    darw_X_axis(painter);
    darw_Y_axis(painter);

    m_cursor_pixmap = QPixmap::fromImage(*m_image);
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
    double k = (m_image_width - xSpace) / (m_xend - m_xstart);
    double pointX = 0;
    double zoomIn = m_xend - m_xstart;

    if (zoomIn >= 1600)
    {
        for (int i = 0; i < zoomIn;)
        {
            int tempX = 0;
            if (m_xstart > 0)
            {
                tempX = (m_xstart + 1000) / 1000;
                tempX = tempX * 1000;
            }
            else
            {
                tempX = m_xstart / 1000;
                tempX = tempX * 1000;
            }
            double tempDouble = tempX - m_xstart;
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

                if (fabs(m_xend - minX) > ui::esp)
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
            if (m_xstart > 0)
            {
                tempX = (m_xstart + 100) / 100;
                tempX = tempX * 100;
            }
            else
            {
                tempX = m_xstart / 100;
                tempX = tempX * 100;
            }
            double tempDouble = tempX - m_xstart;
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

                if (fabs(m_xend - minX) > ui::esp)
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
            if (m_xstart > 0)
            {
                tempX = (m_xstart + 10) / 10;
                tempX = tempX * 10;
            }
            else
            {
                tempX = m_xstart / 10;
                tempX = tempX * 10;
            }

            double tempDouble = tempX - m_xstart;
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

                if (fabs(m_xend - minX) > ui::esp)
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
            if(m_xstart >= 0)
            {
                tempX = m_xstart;
                tempDouble = m_xstart - tempX;
                tempDouble = 1 - tempDouble;
                if (tempX == m_xstart)
                {
                    textNum = m_xstart;
                }
                else
                {
                    textNum = m_xstart + 1;
                }
            }
            else
            {
                tempX = m_xstart;
                tempDouble = tempX - m_xstart;
                textNum = m_xstart;
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

                if (fabs(m_xend - minX) > ui::esp)
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
            if(m_xstart >= 0)
            {
                tempX = toDouble_1(m_xstart);
                tempDouble = tempX - m_xstart + 0.1;
                if (tempX == m_xstart)
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
                tempX = toDouble_1(m_xstart);
                tempDouble = tempX - m_xstart;
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

                if (fabs(m_xend - minX) > ui::esp)
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
    double k = (m_image_heigth - 20) / (m_yend - m_ystart);
    double pointY = 0;
    double zoomIn = m_yend - m_ystart;

    if (zoomIn >= 1600)
    {
        for (int i = 0; i < (m_yend - m_ystart);)
        {
            int tempY = 0;
			double tempDouble = 0;
            if (m_yend > 0)
            {
                tempY = m_yend / 1000;
                tempY = tempY * 1000;
                tempDouble = m_yend - tempY;
            }
            else
            {
                tempY = (m_yend - 1000) / 1000;
                tempY = tempY * 1000;
                tempDouble = - (tempY - m_yend);
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

                if (fabs(m_xstart - minX) > ui::esp)
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
        for (int i = 0; i < (m_yend - m_ystart);)
        {
            int tempY = 0;
			double tempDouble = 0;
            if (m_yend > 0)
            {
                tempY = m_yend / 100;
                tempY = tempY * 100;
                tempDouble = m_yend - tempY;
            }
            else
            {
                tempY = (m_yend - 100) / 100;
                tempY = tempY * 100;
                tempDouble = - (tempY - m_yend);
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

                if (fabs(m_xstart - minX) > ui::esp)
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
         for (int i = 0; i < (m_yend - m_ystart);)
        {
            int tempY = 0;
			double tempDouble = 0;
            if (m_yend > 0)
            {
                tempY = m_yend / 10;
                tempY = tempY * 10;
                tempDouble = m_yend - tempY;
            }
            else
            {
                tempY = (m_yend - 10) / 10;
                tempY = tempY * 10;
                tempDouble = - (tempY - m_yend);
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

                if (fabs(m_xstart - minX) > ui::esp)
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
           for (int i = 0; i < (m_yend - m_ystart);)
        {
            int tempY = 0;
			double tempDouble = 0;
            if (m_yend > 0)
            {
                tempY = m_yend;
                tempDouble = m_yend - tempY;
            }
            else
            {
                tempY = m_yend;
                tempDouble = - (tempY - m_yend);
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

                if (fabs(m_xstart - minX) > ui::esp)
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
		
        for (double i = 0; i < (m_yend - m_ystart);)
        {
            double tempY = 0;
			double tempDouble = 0;
            if (m_yend > 0)
            {
                tempY = toDouble_1(m_yend);
                tempDouble = m_yend - tempY;
            }
            else
            {
                tempY = toDouble_1(m_yend);
                if(tempY == m_yend)
				{
					;
				}
				else
				{
					tempY = tempY + 0.1;
				}
				
                tempDouble = - (tempY - m_yend);
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

                if (fabs(m_xstart - minX) > ui::esp)
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

void ScaleFrame::drawDefectPoint(double x, double y)
{
    m_render_frame->set_defect_point(x, y);
}

void ScaleFrame::slot_set_pen_width(QString width)
{
    m_draw_widget->setWidth(width);
}

render::LayoutView ScaleFrame::load_file(const QString & filename, const QString &dirpath, bool add_layout_view)
{
    return m_render_frame->load_file(filename.toStdString(), dirpath.toStdString(), add_layout_view);
}

render::RenderFrame *ScaleFrame::getRenderFrame()
{
    return m_render_frame;
}

void ScaleFrame::setImageSize(QSize size)
{
    m_image_width = size.width();
    m_image_heigth = size.height();
}

void ScaleFrame::updateMouseCursor(QPoint pos)
{
    m_cursor_pixmap = QPixmap::fromImage(*m_image);
    QPainter painter(&m_cursor_pixmap);
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
    m_xstart = left;
    m_xend = right;
    m_ystart = bot;
    m_yend = top;
    paintImage();
    update();
}

void ScaleFrame::slot_pos_updated(double x, double y)
{
    m_current_posX = x;
    m_current_posY = y;
    emit signal_pos_updated(x, y);
}

void ScaleFrame::slot_distance_updated(double distance)
{
    emit signal_updataDistance(distance);
}

void ScaleFrame::slot_set_painter_style(Global::PaintStyle paintStyle)
{
    m_draw_widget->setPaintStyle(paintStyle);
}

void ScaleFrame::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.drawPixmap(0, 0, m_cursor_pixmap);
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

