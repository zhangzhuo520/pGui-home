#include "ui_paint_widget.h"
namespace ui {
PaintWidget::PaintWidget(QWidget *parent):
    weight(5),
    style(1),
    m_first_point_find(false)
{
    setParent(parent);
    setObjectName("PaintWidget");
    setAttribute(Qt::WA_PaintOutsidePaintEvent);
    setAutoFillBackground (true);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    m_empty_image = new QImage(size(), QImage::Format_ARGB32);
    m_empty_image->fill(Qt::transparent);
    m_mouse_clicks = LineEnd;
    setColor(Qt::black);
    m_ruler_image = *m_empty_image;
    m_defectpoint_size_image = *m_empty_image;
    m_dotted_box_image = *m_empty_image;
    m_mark_cross_image = *m_empty_image;
    m_cross_line_image = *m_empty_image;
    m_select_mode = Global::Nothing;
    m_snap_flag = Global::SnapClose;
}

void PaintWidget::setStyle (int s)
{
    style = s;
}

void PaintWidget::setWidth (QString w)
{
    weight = w.toInt();
}

void PaintWidget::setColor (QColor c)
{
    color = c;
}

void PaintWidget::set_snap_flag(Global::SnapFLag snapflag)
{
    m_snap_flag = snapflag;
}

const QList<LineData> &PaintWidget::get_measure_line_list()
{
    return m_measure_point.get_point_list();
}

void PaintWidget::set_measure_line_list(const QList<LineData> & m_line_list)
{
    m_measure_point.set_point_list(m_line_list);
    repaint_normal_ruler();
}

void PaintWidget::use_angle()
{
    if (m_select_mode == Global::MeasureAngle && m_snap_flag == Global::SnapClose)
    {
        double angle_range = (m_ruler_last_point.y() - m_ruler_first_point.y()) / (m_ruler_last_point.x() - m_ruler_first_point.x());
        if (-0.2 <= angle_range && angle_range < 0.2)
        {
           m_ruler_last_point.setY(m_ruler_first_point.y());
        }
        else if (0.2 <= angle_range && angle_range < 3)
        {
           m_ruler_last_point.setY(m_ruler_first_point.y() + m_ruler_last_point.x() - m_ruler_first_point.x());
        }
        else if (-3 <=angle_range &&  angle_range< -0.2)
        {
           m_ruler_last_point.setY(m_ruler_first_point.y() - m_ruler_last_point.x() + m_ruler_first_point.x());
        }
        else if (3 <= angle_range || angle_range < -3)
        {
            m_ruler_last_point.setX(m_ruler_first_point.x());
        }
        m_end_pos = calcu_physical_point(m_ruler_last_point);
    }
}

void PaintWidget::setPaintStyle(Global::PaintTool paintstyle)
{
    m_select_mode = paintstyle;
    m_mouse_clicks = LineEnd;
}

QPointF PaintWidget::calcu_physical_point(QPointF pos)
{
    QPointF tempPoint;
    tempPoint.setX((double)pos.x() / (double)width() * (m_scale_xend - m_scale_xstart) + m_scale_xstart);
    tempPoint.setY(m_scale_yend - ((double)pos.y() / (double)height() * (m_scale_yend - m_scale_ystart)));
    return tempPoint;
}

QPointF PaintWidget::calcu_pixel_point(QPointF pos)
{
    double m_xratio = (pos.x() - m_scale_xstart) / (m_scale_xend - m_scale_xstart);
    double m_yratio = (m_scale_yend - pos.y()) / (m_scale_yend - m_scale_ystart);
    return QPointF (m_xratio * width(), m_yratio * height());
}

void PaintWidget::mousePressEvent (QMouseEvent *e)
{
    m_current_mousepos = e->pos();
    m_dotted_box_end = m_dotted_box_start = e->pos();

    switch (e->button()) {
    case Qt::LeftButton:
    {
        if (m_select_mode == Global::Nothing)
        {
            draw_cross_line(m_current_mousepos);
        }
        else if (m_select_mode == Global::MeasureLine || m_select_mode == Global::MeasureAngle)
        {
            if(m_mouse_clicks == LineStart)
            {
                m_mouse_clicks = LineEnd;
                if (m_snap_flag == Global::SnapOpen)
                {
                    emit signal_get_snap_pos(m_ruler_last_point.toPoint(), 2);
                }
                else
                {
                    draw_measure_line();
                    merge_image();
                }
            }
            else
            {
                m_mouse_clicks = LineStart;
                if (m_snap_flag == Global::SnapOpen)
                {
                    emit signal_get_snap_pos(m_current_mousepos, 1);
                }
                else
                {
                    m_ruler_first_point = m_current_mousepos;
                    m_start_pos =  calcu_physical_point(m_current_mousepos);
                }
            }
        }
        else if (m_select_mode == Global::MarkCross)
        {
            draw_mark_cross();
            merge_image();
        }
        else if (m_select_mode == Global::RemoveLine)
        {
            QPointF pos = calcu_physical_point(m_current_mousepos);
            if (m_measure_point.removeLineData(pos))
            {
                emit signal_measure_line_list();   // send a signal let line table to get new line list
                repaint_normal_ruler();

            }
        }
        break;
    }
    case Qt::RightButton:
    {
        m_mouse_clicks = LineEnd;
        break;
    }
    default:
        break;
    }
    QWidget::mousePressEvent(e);
}

void PaintWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::RightButton)
    {
        m_dotted_box_end = e->pos();
        emit signal_moveCenter();
    }
    else if(e->button() == Qt::LeftButton)
    {
    }
    QWidget::mouseReleaseEvent(e);
}

void PaintWidget::mouseMoveEvent (QMouseEvent *e)
{
    m_current_mousepos  = e->pos();
    switch (m_select_mode) {
    case Global::Nothing:
    {
        draw_cross_line(m_current_mousepos);
        merge_image();
        break;
    }
    case Global::MarkCross:
    {
        break;
    }
    case Global::MeasureAngle:
    case Global::MeasureLine:
    {
        if(m_mouse_clicks == LineStart)
        {
            if (m_snap_flag == Global::SnapOpen)
            {
                if (m_select_mode == Global::MeasureAngle)
                {
                    m_ruler_last_point = m_current_mousepos;
                    double angle_range = (m_ruler_last_point.y() - m_ruler_first_point.y()) / (m_ruler_last_point.x() - m_ruler_first_point.x());
                    if (-0.2 <= angle_range && angle_range < 0.2)
                    {
                        m_ruler_last_point.setY(m_ruler_first_point.y());
                    }
                    else if (0.2 <= angle_range && angle_range < 3)
                    {
                        m_ruler_last_point.setY(m_ruler_first_point.y() + m_ruler_last_point.x() - m_ruler_first_point.x());
                    }
                    else if (-3 <=angle_range &&  angle_range< -0.2)
                    {
                        m_ruler_last_point.setY(m_ruler_first_point.y() - m_ruler_last_point.x() + m_ruler_first_point.x());
                    }
                    else if (3 <= angle_range || angle_range < -3)
                    {
                        m_ruler_last_point.setX(m_ruler_first_point.x());
                    }
                    m_current_mousepos = m_ruler_last_point.toPoint();
                }
                emit signal_get_snap_pos(m_current_mousepos, 2);
            }
            else
            {
                m_ruler_last_point = m_current_mousepos;
                m_end_pos = calcu_physical_point(m_current_mousepos);
                draw_measure_line();
                merge_image();
                double dx = m_end_pos.x() - m_start_pos.x();
                double dy = m_end_pos.y() - m_start_pos.y();
                m_distance = sqrt( dx * dx + dy * dy);
                emit signal_updateDistance(m_distance);
            }
        }
        break;
    }
    case Global::RemoveLine:
    {
        QPointF pos = calcu_physical_point(m_current_mousepos);
        int index =  m_measure_point.get_select_lineindex(pos);

        if (index != -1)
        {
            for (int j = 0; j < m_measure_point.get_point_list().count(); j ++)
            {
                if (j == index)
                {
                    m_measure_point.get_point_list()[j].set_line_width(3);
                }
                else
                {
                    m_measure_point.get_point_list()[j].set_line_width(1);
                }
            }
        }
        else
        {
            for (int j = 0; j < m_measure_point.get_point_list().count(); j ++)
            {
                m_measure_point.get_point_list()[j].set_line_width(1);
            }
        }
        repaint_normal_ruler();
    }
    default:
        break;
    }

    QWidget::mouseMoveEvent(e);
}

void PaintWidget::paintEvent (QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setRenderHint( QPainter::Antialiasing, true ); //Improve the quality of painting
    painter.drawPixmap(QPoint(0,0), QPixmap::fromImage(m_paint_image));
    QWidget::paintEvent(e);
}

void PaintWidget::resizeEvent (QResizeEvent *event)
{
    *m_empty_image =  (*m_empty_image).scaled(size());
    m_defectpoint_size_image = m_defectpoint_size_image.scaled(size());
    m_ruler_image = m_ruler_image.scaled(size());
    m_mark_cross_image = m_mark_cross_image.scaled(size());
    m_cross_line_image = m_cross_line_image.scaled(size());

    m_defectpoint_size_image.fill(0);
    m_ruler_image.fill(0);
    m_mark_cross_image.fill(0);
    m_cross_line_image.fill(0);

    QWidget::resizeEvent (event);
    event->ignore();
}

void PaintWidget::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
    {
         m_mouse_clicks = LineEnd;
         merge_image();
    }
    QWidget::keyPressEvent(e);
    e->ignore();
}

void PaintWidget::slot_measure_clear ()
{
    m_ruler_image.fill(Qt::transparent);
    m_measure_point.clear_all_data();
    emit signal_measure_line_list();
    merge_image();
}

void PaintWidget::slot_mark_point()
{
    m_mark_cross_image.fill(Qt::transparent);
    m_mark_cross_list.clear();
    merge_image();
}

void PaintWidget::slot_box_updated(double left, double bot, double right, double top)
{
    repaint_image(left, right, bot, top);
}

void PaintWidget::draw_defect_point_text(double x, double y, QString s_size)
{
    if (s_size.isEmpty())
    {
        return;
    }
    m_isdraw_defect_points = true;
    QPen pen;
    m_defectpoint_size_image = *m_empty_image;
    QPainter painter(&m_defectpoint_size_image);
    pen.setStyle ((Qt::PenStyle)style);
    pen.setWidth (weight);
    pen.setColor (color);
    painter.setPen(pen);
    m_defect_size = "size: " + s_size;
    m_defect_x = x;
    m_defect_y = y;
    QPointF p = calcu_pixel_point(QPointF(m_defect_x, m_defect_y));
    painter.drawText(p.x(), p.y() - 10, m_defect_size);
    merge_image();
}

void PaintWidget::draw_cross_line(const QPoint& pos)
{
    m_cross_line_image = *m_empty_image;
    QPainter painter(&m_cross_line_image);

    QPen pen;
    pen.setWidth (weight);
    pen.setColor (color);
    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);
    painter.drawLine(pos.x(), 0, pos.x(), height());
    painter.drawLine(0, pos.y(), width(), pos.y());
}

void PaintWidget::draw_dotted_box()
{
    m_dotted_box_image = *m_empty_image;
    QPen pen;
    pen.setColor(color);
    pen.setWidth(weight);
    pen.setStyle(Qt::DotLine);

    QPainter tempPainter(&m_dotted_box_image);
    tempPainter.setPen(pen);
    tempPainter.drawRoundRect(QRect(m_dotted_box_start, m_dotted_box_end), 0, 0);
}

void PaintWidget::draw_mark_cross()
{
    QPointF tempPos = calcu_physical_point(m_current_mousepos);
    QString currentPosText ='(' + QString::number(tempPos.x(),'f',4) + ',' + QString::number(tempPos.y(),'f',4) + ')';
    QPen pen;
    pen.setStyle ((Qt::PenStyle)style);
    pen.setWidth (3);
    pen.setColor (color);
    QRectF rectangle(m_current_mousepos.x(), m_current_mousepos.y(), 6, 6);

    QPainter painter(this);
    painter.drawRoundedRect(rectangle, 20.0, 15.0);
    QPainter tempPainter(&m_mark_cross_image);
    tempPainter.setRenderHint(QPainter::Antialiasing, true);
    tempPainter.drawText(QPoint(m_current_mousepos.x() - 10, m_current_mousepos.y() - 5), currentPosText);
    tempPainter.setBrush(QBrush(Qt::red, Qt::SolidPattern));
    tempPainter.drawRoundedRect(rectangle, 20, 20);
    m_mark_cross_list.append(tempPos);
}

void PaintWidget::draw_measure_line()
{
    use_angle();
    double arrow_lenght_ = 13;
    double arrow_degrees_ = 0.6;

    double angle = atan2(m_ruler_last_point.y() - m_ruler_first_point.y(), m_ruler_last_point.x() - m_ruler_first_point.x()) + 3.1415926;//

    QPointF rightArrowPoint;
    QPointF leftArrowPoint;

    rightArrowPoint.setX(m_ruler_last_point.x() + arrow_lenght_ * cos(angle - arrow_degrees_));
    rightArrowPoint.setY(m_ruler_last_point.y() + arrow_lenght_ * qSin(angle - arrow_degrees_));
    leftArrowPoint.setX(m_ruler_last_point.x() + arrow_lenght_ * qCos(angle + arrow_degrees_));
    leftArrowPoint.setY(m_ruler_last_point.y() + arrow_lenght_ * qSin(angle + arrow_degrees_));

    if (m_mouse_clicks == LineStart)
    {
        m_ruler_save_image = m_ruler_image;
        QPen pen;
        pen.setStyle ((Qt::PenStyle)style);
        pen.setWidth (weight);
        pen.setColor (color);
        QPainter tempPainter(&m_ruler_save_image);
        tempPainter.setRenderHint(QPainter::Antialiasing, true);
        tempPainter.setPen(pen);
        tempPainter.drawText(QPoint(m_ruler_last_point.x() - 10, m_ruler_last_point.y() - 5), QString::number(m_distance,'f', 6));
        tempPainter.drawLine(m_ruler_first_point, m_ruler_last_point);
        tempPainter.drawLine(m_ruler_last_point, rightArrowPoint);
        tempPainter.drawLine(m_ruler_last_point, leftArrowPoint);
    }
    else
    {
        QPen pen;
        pen.setStyle ((Qt::PenStyle)style);
        pen.setWidth (weight);
        pen.setColor (color);
        QPainter tempPainter(&m_ruler_image);
        tempPainter.setRenderHint(QPainter::Antialiasing, true);
        tempPainter.setPen(pen);
        tempPainter.drawText(QPoint(m_ruler_last_point.x() - 10, m_ruler_last_point.y() - 5), QString::number(m_distance,'f', 6));
        tempPainter.drawLine(m_ruler_first_point, m_ruler_last_point);
        tempPainter.drawLine(m_ruler_last_point, rightArrowPoint);
        tempPainter.drawLine(m_ruler_last_point, leftArrowPoint);
        LineData tempLine(m_start_pos, m_end_pos, m_distance);
        m_measure_point.appendLineData(tempLine);
        emit signal_measure_line_list();
    }
}

void PaintWidget::repaint_normal_ruler()
{
    QList <LineData> listLine = m_measure_point.get_point_list();
    m_ruler_image = *m_empty_image;
    m_ruler_save_image = *m_empty_image;

    if (m_mouse_clicks == LineStart)
    {
        m_ruler_first_point = calcu_pixel_point(m_start_pos);
        m_ruler_last_point = calcu_pixel_point(m_end_pos);

        double arrow_lenght_ = 13;
        double arrow_degrees_ = 0.6;

        double angle = atan2(m_ruler_last_point.y() - m_ruler_first_point.y(), m_ruler_last_point.x() - m_ruler_first_point.x()) + 3.1415926;//

        QPointF rightArrowPoint;
        QPointF leftArrowPoint;

        rightArrowPoint.setX(m_ruler_last_point.x() + arrow_lenght_ * cos(angle - arrow_degrees_));
        rightArrowPoint.setY(m_ruler_last_point.y() + arrow_lenght_ * qSin(angle - arrow_degrees_));
        leftArrowPoint.setX(m_ruler_last_point.x() + arrow_lenght_ * qCos(angle + arrow_degrees_));
        leftArrowPoint.setY(m_ruler_last_point.y() + arrow_lenght_ * qSin(angle + arrow_degrees_));
        QPainter tempPainter(&m_ruler_save_image);
        tempPainter.setRenderHint(QPainter::Antialiasing, true);
        QPen pen;
        pen.setStyle ((Qt::PenStyle)style);
        pen.setWidth (weight);
        pen.setColor (color);
        tempPainter.setPen(pen);
        tempPainter.drawText(QPoint(m_ruler_last_point.x() - 10, m_ruler_last_point.y() - 5), QString::number(m_distance,'f', 6));
        tempPainter.drawLine(m_ruler_first_point, m_ruler_last_point);
        tempPainter.drawLine(m_ruler_last_point, rightArrowPoint);
        tempPainter.drawLine(m_ruler_last_point, leftArrowPoint);
    }
    else
    {
        for (int i = 0; i < listLine.count(); i ++)
        {
            m_ruler_first_point = calcu_pixel_point(listLine.at(i).m_first_point);
            m_ruler_last_point = calcu_pixel_point(listLine.at(i).m_last_point);
            m_distance = listLine.at(i).m_distance;

            double arrow_lenght_ = 13;
            double arrow_degrees_ = 0.6;

            double angle = atan2(m_ruler_last_point.y() - m_ruler_first_point.y(), m_ruler_last_point.x() - m_ruler_first_point.x()) + 3.1415926;//

            QPointF rightArrowPoint;
            QPointF leftArrowPoint;

            rightArrowPoint.setX(m_ruler_last_point.x() + arrow_lenght_ * cos(angle - arrow_degrees_));
            rightArrowPoint.setY(m_ruler_last_point.y() + arrow_lenght_ * qSin(angle - arrow_degrees_));
            leftArrowPoint.setX(m_ruler_last_point.x() + arrow_lenght_ * qCos(angle + arrow_degrees_));
            leftArrowPoint.setY(m_ruler_last_point.y() + arrow_lenght_ * qSin(angle + arrow_degrees_));
            QPen pen;
            pen.setStyle ((Qt::PenStyle)style);
            pen.setWidth (listLine.at(i).m_line_width);
            pen.setColor (listLine.at(i).m_line_color);
            QPainter tempPainter(&m_ruler_image);
            tempPainter.setRenderHint(QPainter::Antialiasing, true);
            tempPainter.setPen(pen);

            tempPainter.drawText(QPoint(m_ruler_last_point.x() - 10, m_ruler_last_point.y() - 5), QString::number(m_distance,'f', 6));
            tempPainter.drawLine(m_ruler_first_point, m_ruler_last_point);
            tempPainter.drawLine(m_ruler_last_point, rightArrowPoint);
            tempPainter.drawLine(m_ruler_last_point, leftArrowPoint);
        }
    }
    merge_image();
}

void PaintWidget::repaint_mark_cross()
{
    m_mark_cross_image = *m_empty_image;

    QPen pen;
    pen.setStyle ((Qt::PenStyle)style);
    pen.setWidth (weight);
    pen.setColor (color);
    QPainter tempPainter(&m_mark_cross_image);
    tempPainter.setRenderHint(QPainter::Antialiasing, true);
    for (int i = 0; i < m_mark_cross_list.count(); i ++)
    {
        QPointF tempPos = calcu_pixel_point(m_mark_cross_list.at(i));
        QString currentPosText ='(' + QString::number(m_mark_cross_list.at(i).x(),'f',4) + ',' + QString::number(m_mark_cross_list.at(i).y(),'f',4) + ')';
        QRectF rectangle(tempPos.x(), tempPos.y(), 6, 6);
        tempPainter.drawText(QPoint(tempPos.x() - 10, tempPos.y() - 5), currentPosText);
        tempPainter.setBrush(QBrush(Qt::red, Qt::SolidPattern));
        tempPainter.drawRoundedRect(rectangle, 20, 20);
    }
    merge_image();
}

void PaintWidget::repaint_defect_point()
{
    if(m_isdraw_defect_points)
    {
        QPen pen;
        m_defectpoint_size_image = *m_empty_image;
        QPainter painter(&m_defectpoint_size_image);
        pen.setStyle ((Qt::PenStyle)style);
        pen.setWidth (weight);
        pen.setColor (color);
        painter.setPen(pen);
        QPointF p = calcu_pixel_point(QPointF(m_defect_x, m_defect_y));
        painter.drawText(p.x(), p.y() - 10, m_defect_size);
        merge_image();
    }

}

void PaintWidget::repaint_image(double m_xstart, double m_xend, double m_ystart, double m_yend)
{
    m_scale_xstart = m_xstart;
    m_scale_xend = m_xend;
    m_scale_ystart = m_ystart;
    m_scale_yend = m_yend;

    repaint_normal_ruler();
    repaint_mark_cross();
    repaint_defect_point();
}

void PaintWidget::slot_get_snap_pos(bool find, QPoint pix_p, QPointF micron_p, int mode)
{
    if(mode == 1)
    {
        if(find)
        {
            m_start_pos =  micron_p;
            m_ruler_first_point = pix_p;
            m_first_point_find = true;
        }
        else
        {
           m_ruler_first_point = m_current_mousepos;
            m_first_point_find = false;
        }
    }
    else if(mode == 2)
    {
        m_end_pos = micron_p;
        m_ruler_last_point = pix_p;
        double dx = m_end_pos.x() - m_start_pos.x();
        double dy = m_end_pos.y() - m_start_pos.y();
        m_distance = sqrt( dx * dx + dy * dy);
        emit signal_updateDistance(m_distance);

        if(find)
        {
            if (m_first_point_find)
            {
                draw_measure_line();
                merge_image();
            }
        }
        else
        {
            m_ruler_last_point = m_current_mousepos;
            m_mouse_clicks = LineStart;
            if (m_first_point_find)
            {
                draw_measure_line();
                merge_image();
            }
        }
    }
}

QImage PaintWidget::merge_two_images(const QImage& baseImage, const QImage& overlayImage)
{
    QImage imageWithOverlay = QImage(baseImage.size(), QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&imageWithOverlay);

    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(imageWithOverlay.rect(), Qt::transparent);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, baseImage);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, overlayImage);

    painter.end();

    return imageWithOverlay;
}

void PaintWidget::merge_image()
{
    switch (m_select_mode) {
    case Global::Nothing:
    {
        m_paint_image = merge_two_images(m_mark_cross_image, m_cross_line_image);
        m_paint_image = merge_two_images(m_paint_image, m_ruler_image);
        m_paint_image = merge_two_images(m_defectpoint_size_image, m_paint_image);
        break;
    }
    case Global::MarkCross:
    {
        m_paint_image = merge_two_images(m_ruler_image, m_mark_cross_image);
        m_paint_image = merge_two_images(m_defectpoint_size_image, m_paint_image);
        break;
    }
    case Global::MeasureAngle:
    case Global::RemoveLine:
    case Global::MeasureLine:
    {
        if (m_mouse_clicks == LineStart)
        {
            m_paint_image = merge_two_images(m_mark_cross_image, m_defectpoint_size_image);
            m_paint_image = merge_two_images(m_paint_image, m_ruler_save_image);
        }
        else
        {
            m_paint_image = merge_two_images(m_mark_cross_image, m_defectpoint_size_image);
            m_paint_image = merge_two_images(m_paint_image, m_ruler_image);
        }
        break;
    }
    default:
            break;
        }
    update();
}
}
