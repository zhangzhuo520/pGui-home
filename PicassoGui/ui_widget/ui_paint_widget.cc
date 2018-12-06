#include "ui_paint_widget.h"
namespace ui {
PaintWidget::PaintWidget(QWidget *parent):
    QWidget(parent),
    weight(5),
    style(1)
{
    setObjectName("PaintWidget");
    setAutoFillBackground (true);
    setMouseTracking(true);
    m_empty_image = new QImage(size(), QImage::Format_RGB32);
    m_empty_image->fill(Qt::transparent);
    m_mouse_clicks = LineEnd;
    m_paint_image = *m_empty_image;

    setColor(Qt::white);
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

void PaintWidget::mousePressEvent (QMouseEvent *e)
{
    m_current_mousepos = e->pos();
    m_dotted_box_end = m_dotted_box_start = e->pos();
    m_mouse_state = Press;

    switch (e->button()) {
    case Qt::RightButton:
    {
        if (m_select_mode == Global::Normal)
        {
            draw_cross_line(m_current_mousepos);
        }
        else if (m_select_mode == Global::Measrue)
        {
            if(m_mouse_clicks == LineEnd)
            {
                m_mouse_clicks = LineStart;
                m_ruler_first_point = m_ruler_last_point = e->pos();
            }
            else
            {
                m_mouse_clicks = LineStart;
                m_ruler_last_point = e->pos();
                LineData tempLine;
                tempLine.m_first_point = m_ruler_first_point;
                tempLine.m_last_point = m_ruler_last_point;
                m_measure_point.appendLineData(tempLine);
            }

        }
        else if (m_select_mode == Global::Mark)
        {

        }
        break;
    }
    case Qt::LeftButton:
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
        m_ruler_last_point = e->pos();
    }
    setMouseTracking(true);
    draw_cross_line(e->pos());
    update();
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
            draw_dotted_box();
        }
        else
        {
            draw_cross_line(m_current_mousepos);
            update();
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
            drawRuler();
        }
        break;
    }
    default:
        break;
    }

//    if (checkstyle == RubberBand)
//    {
//        m_dotted_box_endpoint = e->pos();
//        drawRubberRect();
//        return;
//    }
//    else if (penStyle == Global::RulerStyle && rulerIsPress)
//    {
//        m_ruler_last_point = e->pos();
//        endPos = e->pos();
//        drawRuler();
//        signal_mouseMove(e->pos());
//        emit signal_updataDistance((m_ruler_last_point.x() - m_ruler_first_point.x())/(m_ruler_last_point.y() - m_ruler_first_point.y()) );
//        emit signal_distancePoint(startPos, endPos);
//        return;
//    }
//    else
//    {
//        paint_cross_line(e->pos());
//    }
    update();
    QWidget::mouseMoveEvent(e);
}

void PaintWidget::paintEvent (QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setRenderHint( QPainter::Antialiasing, true ); //Improve the quality of painting
    painter.drawPixmap(QPoint(0,0), QPixmap::fromImage(m_crossline_image));
    QWidget::paintEvent(e);
}

void PaintWidget::resizeEvent (QResizeEvent *event)
{
//    if (height () > pix->height () || width () > pix->width ())
//    {
//        QPixmap *newPix = new QPixmap(size());
//        newPix->fill (Qt::transparent);
//        QPainter p(newPix);
//        p.drawPixmap (QPoint(0, 0), *pix);
//        pix = newPix;
//    }
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

void PaintWidget::draw_point_text(double x, double y, QString Stringsize)
{
        m_database_text_image = *m_empty_image;
        QPen pen;
        QPainter painter(&m_database_text_image);
        pen.setStyle ((Qt::PenStyle)style);
        pen.setWidth (weight);
        pen.setColor (color);
        painter.setPen(pen);
        Stringsize = "size: " + Stringsize;
        painter.drawText(x * width(), y * height() - 10, Stringsize);
}

void PaintWidget::draw_cross_line(const QPoint& pos)
{
    m_crossline_image = *m_empty_image;
    QPainter painter(&m_crossline_image);
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
        merge_two_images();
        break;
    }
    case Global::Mark:
    {

        break;
    }
    case Global::Measrue:
    {
        break;
    }
    default:
        break;
    }
}

void PaintWidget::drawRuler()
{
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
    tempPainter.drawLine(m_ruler_first_point, m_ruler_last_point);
    tempPainter.drawLine(m_ruler_last_point, rightArrowPoint);
    tempPainter.drawLine(m_ruler_last_point, leftArrowPoint);
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
