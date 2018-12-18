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
    setColor(Qt::white);
    m_ruler_image = *m_empty_image;
    m_defectpoint_size_image = *m_empty_image;
    m_dotted_box_image = *m_empty_image;
    m_select_mode = Global::Normal;
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

void PaintWidget::slot_get_snap_pos(QPoint pos, double x, double y, int mode)
{
    if(mode == 1)
    {
        startPos = endPos = QPointF(x,y);
        m_ruler_first_point = m_ruler_last_point = pos;
    }
    else if(mode == 2)
    {
        endPos = QPointF(x, y);
        m_ruler_last_point = pos;
    }
    drawRuler();
    merge_image();
}

void PaintWidget::mousePressEvent (QMouseEvent *e)
{
    m_current_mousepos = e->pos();
    m_dotted_box_end = m_dotted_box_start = e->pos();
    m_mouse_state = Press;

    switch (e->button()) {
    case Qt::LeftButton:
    {
        if (m_select_mode == Global::Normal)
        {
            draw_cross_line(m_current_mousepos);
        }
        else if (m_select_mode == Global::Measrue)
        {
            if(m_mouse_clicks == LineStart)
            {
                m_mouse_clicks = LineEnd;
                LineData tempLine;
                tempLine.m_first_point = m_ruler_first_point;
                tempLine.m_last_point = m_ruler_last_point;
                emit signal_get_snap_pos(m_current_mousepos, 2);
            }
            else
            {
                m_mouse_clicks = LineStart;
				emit signal_get_snap_pos(m_current_mousepos, 1);
            }
        }
        else if (m_select_mode == Global::Mark)
        {

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
    case Global::Normal:
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
    case Global::Mark:
    {
        break;
    }
    case Global::Measrue:
    {
        if(m_mouse_clicks == LineStart)
        {
            emit signal_get_snap_pos(e->pos(), 2);
            double dx = endPos.x() - startPos.x();
            double dy = endPos.y() - startPos.y();
            distance = sqrt( dx * dx + dy * dy);
            emit signal_updataDistance(distance);
        }
        break;
    }
    default:
        break;
    }

    QWidget::mouseMoveEvent(e);
}

void PaintWidget::paintEvent (QPaintEvent *e)
{
    TIME_DEBUG
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setRenderHint( QPainter::Antialiasing, true ); //Improve the quality of painting
    painter.drawPixmap(QPoint(0,0), QPixmap::fromImage(m_paint_image));
    QWidget::paintEvent(e);
    TIME_DEBUG
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

void PaintWidget::setPaintStyle(Global::PaintStyle paintStyle)
{
    m_select_mode = paintStyle;
}

void PaintWidget::clear ()
{
//    QPixmap *clearPix = new QPixmap(size());
//    clearPix->fill (Qt::transparent);
//    pix = clearPix;
//    update ();
}

void PaintWidget::slot_setPaintStyle(Global::PaintStyle paintstyle)
{
    m_select_mode = paintstyle;
}

void PaintWidget::draw_defect_point_text(double x, double y, QString Stringsize)
{
        QPen pen;
        m_defectpoint_size_image = *m_empty_image;
        QPainter painter(&m_defectpoint_size_image);
        pen.setStyle ((Qt::PenStyle)style);
        pen.setWidth (weight);
        pen.setColor (color);
        painter.setPen(pen);
        Stringsize = "size: " + Stringsize;
        painter.drawText(x * width(), y * height() - 10, Stringsize);
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

void PaintWidget::merge_image()
{
    switch (m_select_mode) {
    case Global::Normal:
    {
        m_paint_image = merge_two_images(m_defectpoint_size_image, m_cross_line_image);
        break;
    }
    case Global::Mark:
    {
        break;
    }
    case Global::Measrue:
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

void PaintWidget::drawRuler()
{

    TIME_DEBUG
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
        qDebug() << "11111111111111111111111111111111111";
        m_ruler_save_image = m_ruler_image;
        QPen pen;
        pen.setStyle ((Qt::PenStyle)style);
        pen.setWidth (weight);
        pen.setColor (color);
        QPainter tempPainter(&m_ruler_save_image);
        tempPainter.setRenderHint(QPainter::Antialiasing, true);
        tempPainter.setPen(pen);
        tempPainter.drawText(QPoint(m_ruler_last_point.x() - 10, m_ruler_last_point.y() - 5), QString::number(distance,'f', 6));
        tempPainter.drawLine(m_ruler_first_point, m_ruler_last_point);
        tempPainter.drawLine(m_ruler_last_point, rightArrowPoint);
        tempPainter.drawLine(m_ruler_last_point, leftArrowPoint);
    }
    else
    {
        qDebug() << "22222222222222222222222";
        QPen pen;
        pen.setStyle ((Qt::PenStyle)style);
        pen.setWidth (weight);
        pen.setColor (color);
        QPainter tempPainter(&m_ruler_image);
        tempPainter.setRenderHint(QPainter::Antialiasing, true);
        tempPainter.setPen(pen);
        tempPainter.drawText(QPoint(m_ruler_last_point.x() - 10, m_ruler_last_point.y() - 5), QString::number(distance,'f', 6));
        tempPainter.drawLine(m_ruler_first_point, m_ruler_last_point);
        tempPainter.drawLine(m_ruler_last_point, rightArrowPoint);
        tempPainter.drawLine(m_ruler_last_point, leftArrowPoint);
    }
    TIME_DEBUG
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
}
