#include "ui_paint_widget.h"
namespace ui {
PaintWidget::PaintWidget(QWidget *parent):
    weight(5),
    style(1)
{
    setParent(parent);
    setObjectName("PaintWidget");
    setAttribute(Qt::WA_PaintOutsidePaintEvent);
    setAutoFillBackground (true);
    setMouseTracking(true);
    m_empty_image = new QImage(size(), QImage::Format_ARGB32);
    m_empty_image->fill(Qt::transparent);
    m_mouse_clicks = LineEnd;
    m_mouse_state = release;
    setColor(Qt::black);
    m_ruler_image = *m_empty_image;
    m_defectpoint_size_image = *m_empty_image;
    m_dotted_box_image = *m_empty_image;
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

void PaintWidget::use_angle()
{
    if (m_select_mode == Global::MeasureAngle)
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
}

void PaintWidget::slot_get_snap_pos(bool find, QPoint pix_p, QPointF micron_p, int mode)
{
    if(mode == 1)
    {
        if(find)
        {
            m_start_pos =  micron_p;
            m_ruler_first_point = pix_p;
        }
    }
    else if(mode == 2)
    {
        if(find)
        {
            m_end_pos = micron_p;
            m_ruler_last_point = pix_p;

            double dx = m_end_pos.x() - m_start_pos.x();
            double dy = m_end_pos.y() - m_start_pos.y();
            m_distance = sqrt( dx * dx + dy * dy);
            emit signal_updateDistance(m_distance);
            drawMeasureLine();
            merge_image();
        }
    }
}

QPointF PaintWidget::calcu_physical_point(QPointF pos)
{
    QPointF tempPoint;
    tempPoint.setX((double)pos.x() / (double)width() * (m_scale_xend - m_scale_xstart) + m_scale_xstart);
    tempPoint.setY(m_scale_yend - ((double)pos.y() / (double)height() * (m_scale_yend - m_scale_ystart)));
    return tempPoint;
}

void PaintWidget::mousePressEvent (QMouseEvent *e)
{
    m_current_mousepos = e->pos();
    m_dotted_box_end = m_dotted_box_start = e->pos();
    m_mouse_state = Press;

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
                    emit signal_get_snap_pos(m_current_mousepos, 2);
                }
                else
                {
                    drawMeasureLine();
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
            //do nothing...
        }
        else if (m_select_mode == Global::RemoveLine)
        {
//            if(m_mouse_clicks == LineStart)
//            {
//                m_mouse_clicks = LineEnd;
//                drawMeasureLine();
//                merge_image();
//            }

            QPointF pos = calcu_physical_point(m_current_mousepos);
            m_measure_point.removeLineData(pos);

        }

        break;
    }
    case Qt::RightButton:
    {
        break;
    }
    default:
        break;
    }
    QWidget::mousePressEvent(e);
}

void PaintWidget::mouseReleaseEvent(QMouseEvent *e)
{
    m_mouse_state = release;
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
        if (m_mouse_state == Press)
        {
          //  draw_dotted_box();
        }
        else
        {
            draw_cross_line(m_current_mousepos);
            merge_image();
        }
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
                emit signal_get_snap_pos(e->pos(), 2);
            }
            else
            {
                m_ruler_last_point = m_current_mousepos;
                m_end_pos = calcu_physical_point(m_current_mousepos);
                drawMeasureLine();
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

        break;
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
    if (height () > m_paint_image.height() || width () > m_paint_image.width ())
    {
        m_defectpoint_size_image = m_defectpoint_size_image.scaled(size());
        m_ruler_image = m_ruler_image.scaled(size());
        *m_empty_image =  (*m_empty_image).scaled(size());
    }
    QWidget::resizeEvent (event);
}

void PaintWidget::clear ()
{
    m_ruler_image.fill(Qt::transparent);
    m_measure_point.clear_all_data();
    merge_image();
}

void PaintWidget::draw_defect_point_text(double x, double y, QString s_size)
{
    if (s_size.isEmpty())
    {
        return;
    }
    QPen pen;
    m_defectpoint_size_image = *m_empty_image;
    QPainter painter(&m_defectpoint_size_image);
    pen.setStyle ((Qt::PenStyle)style);
    pen.setWidth (weight);
    pen.setColor (color);
    painter.setPen(pen);
    Stringsize = "size: " + s_size;
    painter.drawText(x * width(), y * height() - 10, Stringsize);
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

void PaintWidget::drawMeasureLine()
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
        LineData tempLine;
        tempLine.m_first_point = m_start_pos;
        tempLine.m_last_point = m_end_pos;
        tempLine.m_distance = m_distance;
        m_measure_point.appendLineData(tempLine);
    }
}

void PaintWidget::repaint_snap_ruler()
{
    QList <LineData> listLine = m_measure_point.get_point_list();
    QList <QPair<QPointF, QPointF> >result;
    for(int i = 0; i < listLine.count(); i++)
    {
        QPointF p1 = listLine.at(i).m_first_point;
        QPointF p2 = listLine.at(i).m_last_point;
        result.append(QPair<QPointF, QPointF>(p1, p2));
    }
    emit signal_repaint_snap_ruler(result);
}

void PaintWidget::slot_repaint_snap_ruler(QList<QPair<QPointF, QPointF> > result)
{
    for(int i = 0; i < result.count(); i++)
    {
        m_ruler_first_point = result[i].first;
        m_ruler_last_point = result[i].second;
        m_distance = m_measure_point.get_point_list().at(i).m_distance;

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
        pen.setWidth (weight);
        pen.setColor (color);
        QPainter tempPainter(&m_ruler_image);
        tempPainter.setRenderHint(QPainter::Antialiasing, true);
        tempPainter.setPen(pen);
        tempPainter.drawText(QPoint(m_ruler_last_point.x() - 10, m_ruler_last_point.y() - 5), QString::number(m_distance,'f', 6));
        tempPainter.drawLine(m_ruler_first_point, m_ruler_last_point);
        tempPainter.drawLine(m_ruler_last_point, rightArrowPoint);
        tempPainter.drawLine(m_ruler_last_point, leftArrowPoint);
    }
    merge_image();
}

void PaintWidget::repaint_normal_ruler(double m_xstart, double m_xend, double m_ystart, double m_yend)
{
    QList <LineData> listLine = m_measure_point.get_point_list();
    m_ruler_image = *m_empty_image;
    m_ruler_save_image = *m_empty_image;

    if (m_mouse_clicks == LineStart)
    {
        double m_xratio_start = (m_start_pos.x() - m_xstart) / (m_xend - m_xstart);
        double m_yratio_start = (m_yend - m_start_pos.y()) / (m_yend - m_ystart);

        double m_xratio_end = (m_end_pos.x() - m_xstart) / (m_xend - m_xstart);
        double m_yratio_end = (m_yend - m_end_pos.y()) / (m_yend - m_ystart);

        m_ruler_first_point = QPointF (m_xratio_start * width(), m_yratio_start * height());
        m_ruler_last_point = QPointF (m_xratio_end * width(), m_yratio_end * height());

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
            double m_xratio_start = (listLine.at(i).m_first_point.x() - m_xstart) / (m_xend - m_xstart);
            double m_yratio_start = (m_yend - listLine.at(i).m_first_point.y()) / (m_yend - m_ystart);

            double m_xratio_end = (listLine.at(i).m_last_point.x() - m_xstart) / (m_xend - m_xstart);
            double m_yratio_end = (m_yend - listLine.at(i).m_last_point.y()) / (m_yend - m_ystart);

            m_ruler_first_point = QPointF (m_xratio_start * width(), m_yratio_start * height());
            m_ruler_last_point = QPointF (m_xratio_end * width(), m_yratio_end * height());
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
            pen.setWidth (weight);
            pen.setColor (color);
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

void PaintWidget::repaintRuler(double m_xstart, double m_xend, double m_ystart, double m_yend)
{
    m_scale_xstart = m_xstart;
    m_scale_xend = m_xend;
    m_scale_ystart = m_ystart;
    m_scale_yend = m_yend;
/*    if (m_snap_flag == Global::SnapOpen)
    {
        //repaint_snap_ruler();
        repaint_normal_ruler(m_xstart, m_xend, m_ystart, m_yend);
    }
    else if (m_snap_flag == Global::SnapClose)
    {
        repaint_normal_ruler(m_xstart, m_xend, m_ystart, m_yend);
    }
*/
    repaint_normal_ruler(m_xstart,m_xend, m_ystart, m_yend);
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
        m_paint_image = merge_two_images(m_defectpoint_size_image, m_cross_line_image);
        break;
    }
    case Global::MarkCross:
    {
        break;
    }
    case Global::MeasureAngle:
    case Global::MeasureLine:
    {
        if (m_mouse_clicks == LineStart)
        {
            m_paint_image = merge_two_images(m_defectpoint_size_image, m_ruler_save_image);
        }
        else
        {
            m_paint_image = merge_two_images(m_defectpoint_size_image, m_ruler_image);
        }
        break;
    }
    default:
            break;
        }
        update();
    }
}
