#include "ui_scale_frame.h"
#include "../renderer/render_layout_view.h"
#include <QPushButton>

namespace ui
{

ScaleFrame::ScaleFrame(QWidget *parent) :
    QFrame(parent),
//    m_xratio(0),
//    m_yratio(0),
//    m_size_text(""),
    m_isdraw_pointtext(false),
    m_xstart(0),
    m_xend(0),
    m_ystart(0),
    m_yend(0),
    m_image_width(parent->width()),
    m_image_heigth(parent->height()),
    m_current_posX(0),
    m_current_posY(0),
    m_axis_falg(false)
{
    m_hlayout = new QHBoxLayout(this);
    m_hlayout->setContentsMargins(xSpace, ySpace, 0, 0);
    setMouseTracking(true);
    initRenderFrame();
    initImage();
    paintImage();
    setAttribute(Qt::WA_DeleteOnClose, true);
    emit signal_box_updated();
}

void ScaleFrame::initRenderFrame()
{
    m_render_frame = new render::RenderFrame(this);
    m_paint_widget = new PaintWidget(m_render_frame);
    m_render_frame->set_cursor_widget(m_paint_widget);
    m_hlayout->addWidget(m_render_frame);
    connect(m_render_frame, SIGNAL(signal_box_updated(double,double,double,double)), this, SLOT(slot_box_updated(double,double,double,double)));
    connect(m_render_frame, SIGNAL(signal_pos_updated(double,double)), this, SLOT(slot_pos_updated(double, double)));
    connect(m_render_frame, SIGNAL(signal_get_snap_pos(bool, QPointF, QPointF, int)), m_paint_widget, SLOT(slot_get_snap_pos(bool, QPointF, QPointF, int)));
    connect(m_render_frame, SIGNAL(signal_get_snap_edge(bool, QLineF, QLineF, int)), m_paint_widget, SLOT(slot_get_snap_edge(bool, QLineF, QLineF, int)));
    connect(m_render_frame, SIGNAL(signal_box_updated(double,double,double,double)), m_paint_widget, SLOT(slot_box_updated(double,double,double, double)));
    connect(m_render_frame, SIGNAL(signal_layout_view_changed(render::RenderFrame*)), this, SLOT(slot_layout_view_changed(render::RenderFrame*)));


    connect(m_paint_widget, SIGNAL(signal_updateDistance(QString)), this, SLOT(slot_distance_updated(QString)));
    connect(m_paint_widget, SIGNAL(signal_moveCenter()), this, SLOT(slot_move_point_center()));
    connect(m_paint_widget, SIGNAL(signal_get_snap_pos(QPointF, int)), m_render_frame, SLOT(slot_get_snap_pos(QPointF, int)));
    connect(m_paint_widget, SIGNAL(signal_get_snap_edge(QPointF,int)), m_render_frame, SLOT(slot_get_snap_edge(QPointF, int)));
    connect(m_paint_widget, SIGNAL(signal_measure_line_list()), this, SLOT(slot_update_mesuretable()));
    connect(m_paint_widget, SIGNAL(signal_cutline_list()), this, SLOT(slot_update_cutline_table()));
    connect(m_paint_widget, SIGNAL(signal_go_to_position(QPointF)), m_render_frame, SLOT(slot_go_to_position(QPointF)));
    connect(this, SIGNAL(signal_refresh()), m_render_frame, SLOT(slot_refresh()));
    connect(this, SIGNAL(signal_zoom_in()), m_render_frame, SLOT(slot_zoom_in()));
    connect(this, SIGNAL(signal_zoom_out()), m_render_frame, SLOT(slot_zoom_out()));
    connect(this, SIGNAL(signal_box_updated()), m_render_frame, SLOT(slot_box_updated()));
    connect(this, SIGNAL(signal_zoom_fit()), m_render_frame, SLOT(slot_zoom_fit()));
}

void ScaleFrame::slot_move_point_center()
{
    m_render_frame->center_at_point(m_current_posX, m_current_posY);
}

void ScaleFrame::slot_set_snapfalg(Global::SnapFLag snapflag)
{
    m_paint_widget->set_snap_flag(snapflag);
}

void ScaleFrame::slot_clear_all()
{
    m_paint_widget->slot_clear_all();
}

void ScaleFrame::slot_clear_measureline()
{
    m_paint_widget->slot_measure_line_clear();
}

void ScaleFrame::slot_clear_mark_point()
{
    m_paint_widget->slot_mark_point();
}

void ScaleFrame::slot_clear_gauge()
{
    m_paint_widget->clear_gauge_line();
}

void ScaleFrame::slot_update_mesuretable()
{
    emit signal_update_measuretable();
}

void ScaleFrame::slot_update_cutline_table()
{
    emit signal_updata_cutline_table();
}

void ScaleFrame::set_defect_point(double x, double y)
{
    m_render_frame->set_defect_point(x, y);
}

void ScaleFrame::set_center_point(double x, double y, double view_range)
{
    m_render_frame->set_center_point(x, y, view_range);
}

void ScaleFrame::set_center_point(double x, double y)
{
    m_render_frame->center_at_point(x, y);
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
    double pointNum = 0;
    QString str = QString::number(x, 'g', 6);
    QStringList list = str.split('.');
    if (list.count() > 1)
    {
        pointNum = list.at(1).left(1).toDouble() * 0.1;
    }

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
    if (zoomIn >= 960000)
    {
        ;
    }
    else if(zoomIn > 9000 && zoomIn < 96000)
    {
        for (int i = 0; i < zoomIn;)
        {
            int tempX = 0;
            if (m_xstart > 0)
            {
                tempX = (m_xstart + 10000) / 10000;
                tempX = tempX * 10000;
            }
            else
            {
                tempX = m_xstart / 10000;
                tempX = tempX * 10000;
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
                for(double m = 0; m < tempDouble;)
                {
                    int minX = m * k;
                    if ((pointX - minX) >= xSpace)
                    {
                        painter.drawLine(QPoint(pointX - minX, axis_unit_start), QPoint(pointX - minX, axis_unit_short));
                    }
                    m = m + 1000;
                }
            }

            for(int j = 0; j < 10; j ++)
            {
                double minX = pointX + j * k * 1000;

                if (fabs(m_xend - minX) > ui::esp)
                {
                    if (minX >= xSpace)
                    {
                        painter.drawLine(QPoint(minX, axis_unit_start), QPoint(minX, axis_unit_short));
                    }
                }
            }
            i = i + 10000;
        }
    }
    else if (zoomIn >= 1600 && zoomIn <= 9000)
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
                for(double m = 0; m < tempDouble;)
                {
                    int minX = m * k;
                    if ((pointX - minX) >= xSpace)
                    {
                        painter.drawLine(QPoint(pointX - minX, axis_unit_start), QPoint(pointX - minX, axis_unit_short));
                    }
                    m = m + 100;
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
                    int minX = m * k;
                    if ((pointX - minX) >= xSpace)
                    {
                        painter.drawLine(QPoint(pointX - minX, axis_unit_start), QPoint(pointX - minX, axis_unit_short));
                    }
                    m = m + 10;
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

    if (zoomIn > 96000)
    {
        ;
    }
    else if (zoomIn > 9000 && zoomIn < 96000)
    {
        for (int i = 0; i < (m_yend - m_ystart);)
        {
            int tempY = 0;
            double tempDouble = 0;
            if (m_yend > 0)
            {
                tempY = m_yend / 10000;
                tempY = tempY * 10000;
                tempDouble = m_yend - tempY;
            }
            else
            {
                tempY = (m_yend - 10000) / 10000;
                tempY = tempY * 10000;
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
                    m = m + 1000;
                }
            }

            for(int j = 0; j < 10; j ++)
            {
                double minX = pointY + j * k * 1000;

                if (fabs(m_xstart - minX) > ui::esp)
                {
                    if (minX >= 20)
                    {
                        painter.drawLine(QPoint(axis_unit_start, minX), QPoint(axis_unit_short, minX));
                    }
                }
            }
            i = i + 10000;
        }
    }
    else if (zoomIn >= 1600 && zoomIn <= 9000)
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

void ScaleFrame::drawDefectPoint(double x, double y, QString Stringsize)
{
    logger_file("drawDefectPoint");
    m_isdraw_pointtext = true;
    m_render_frame->set_defect_point(x, y);
    m_paint_widget->draw_defect_point_text(x, y, Stringsize);
}

void ScaleFrame::draw_gauge_line(QPointF start, QPointF end, QString info)
{
    m_paint_widget->draw_gauge_line(start, end, info);
}

void ScaleFrame::get_canvas_coord(double * left, double *right, double *bottom, double * top)
{
    *left = m_xstart;
    *right = m_xend;
    *bottom = m_yend;
    *top = m_ystart;
}

void ScaleFrame::repaint_image()
{
    m_paint_widget->repaint_image(m_xstart, m_xend, m_ystart, m_yend);
}

void ScaleFrame::is_show_axis(bool flag)
{
    m_axis_falg = flag;
    if (flag)
    {
        layout()->setContentsMargins(20, 20, 0, 0);
    }
    else
    {
        layout()->setContentsMargins(0, 0, 0, 0);
    }
}

void ScaleFrame::slot_set_pen_width(QString width)
{
    m_paint_widget->setWidth(width);
}

void ScaleFrame::slot_set_pen_color(const QColor& color)
{
    m_paint_widget->setColor(color);
}

int ScaleFrame::overlay_times = 0;

render::RenderFrame *ScaleFrame::getRenderFrame()
{
    return m_render_frame;
}

const QList<LineData*> & ScaleFrame::get_measure_line_list()
{
    return m_paint_widget->get_measure_line_list();
}

void ScaleFrame::set_measure_line_list(const QList<LineData*> & line_list)
{
    m_paint_widget->set_measure_line_list(line_list);
}

const QList<LineData*> &ScaleFrame::get_cutline_list()
{
    return m_paint_widget->get_cutline_list();
}

void ScaleFrame::set_cutline_list(const QList<LineData*> & list)
{
    m_paint_widget->set_cutline_list(list);
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

void ScaleFrame::slot_distance_updated(QString distance)
{
    emit signal_updateDistance(distance);
}

void ScaleFrame::slot_set_painter_style(Global::PaintTool paintStyle)
{
    m_paint_widget->set_paint_style(paintStyle);
}

void ScaleFrame::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    if (m_axis_falg)
    {
        QPainter painter(this);
        painter.drawPixmap(0, 0, m_cursor_pixmap);
    }
}

void ScaleFrame::resizeEvent(QResizeEvent *e)
{
    setImageSize(e->size());
    initImage();
    paintImage();
    update();
    e->ignore();
}

void ScaleFrame::mouseMoveEvent(QMouseEvent *e)
{
    updateMouseCursor(e->pos());
    update();
}

void ScaleFrame::wheelEvent(QWheelEvent *e)
{
    updateMouseCursor(e->pos());
    update();
}

void ScaleFrame::zoom_in()
{
    emit signal_zoom_in();
}

void ScaleFrame::zoom_out()
{
    emit signal_zoom_out();
}

void ScaleFrame::refresh()
{
    emit signal_refresh();
    emit signal_box_updated();
}

void ScaleFrame::zoom_fit()
{
    emit signal_zoom_fit();
}

void ScaleFrame::slot_layout_view_changed(render::RenderFrame* frame)
{
    if(frame->layout_views_size() == 1)
    {
        m_filename = QString::fromStdString(frame->get_layout_view(0)->file_name());
    }
    else if(frame->layout_views_size() > 1)
    {
        set_file_name(QString("Append[%1]").arg(frame->layout_views_size()));
    }

    for(auto i = 0; i < m_frame_info.fileinfo_vector.count(); i ++)
    {
        render::LayoutView * layout_view = m_frame_info.fileinfo_vector[i].layout_view;
        bool is_exist = false;
        for (auto j = 0; j < frame->layout_views_size(); j ++)
        {
            if (layout_view == frame->get_layout_view(j))
            {
                is_exist = true;
            }
        }

        if (!is_exist)
        {
            m_frame_info.fileinfo_vector.remove(i);
        }
    }

    emit signal_layout_view_changed(frame);
}

void ScaleFrame::closeEvent(QCloseEvent *e)
{
    qDebug() << "close ScaleFrame";
    QFrame::closeEvent(e);
    e->ignore();
}

QVector<QString> ScaleFrame::get_file_name_list()
{
    QVector<QString> result;
    if(m_render_frame)
    {
        for(int i = 0; i < m_render_frame->layout_views_size(); i++)
        {
            QString filename = QString::fromStdString(m_render_frame->get_layout_view(i)->file_name());
            result.append(filename);
        }
    }
    return result;
}

double ScaleFrame::get_view_range() const
{
    return m_render_frame->get_view_range();
}

void ScaleFrame::slot_set_background_color(QColor color)
{
    m_render_frame->set_background_color(color);
}

void ScaleFrame::slot_paint_cutline(Global::RtsCutLineAngle angel_mode, QVariant data)
{
    m_paint_widget->set_paint_style(Global::CutLine);
    m_paint_widget->set_rts_cutline(angel_mode, data);
}

}

