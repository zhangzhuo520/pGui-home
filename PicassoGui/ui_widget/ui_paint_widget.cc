#include "ui_paint_widget.h"
namespace ui {
PaintWidget::PaintWidget(QWidget *parent):
    QWidget(parent),
    m_is_drawtext(false),
    weight(5),
    style(1),
    checkstyle(Normal),
    rulerIsPress(false)
{
    setObjectName("PaintWidget");
    setAutoFillBackground (true);
    setMouseTracking(true);
    pix = new QPixmap(size());
    pix->fill(Qt::transparent);
    m_text_pix = new QPixmap(size());
    m_text_pix->fill(Qt::transparent);
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
    currentPos = e->pos();
    RubberEndPoint = RubberStartPoint = e->pos();
    if (e->button() == Qt::RightButton)
    {
        checkstyle = RubberBand;
        if (hasMouseTracking())
        {
            setMouseTracking(false);
        }
        else
        {
            setMouseTracking(true);
            update();
        }
    }
    else if (e->button() == Qt::LeftButton)
    {
        if (penStyle == Global::MouseStyle)
        {
            setCursor(Qt::CrossCursor);
        }
        else if (penStyle == Global::PenStyle)
        {
            setCursor(Qt::PointingHandCursor);
        }
        else if (penStyle == Global::RulerStyle)
        {
            setCursor(Qt::ArrowCursor);
            lineStartPoint = lineEndPoint = e->pos();

            startPos = endPos = currentPos;
            rulerIsPress = true;
        }
    }
     QWidget::mousePressEvent(e);
}

void PaintWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::RightButton)
    {
        RubberEndPoint = e->pos();
        checkstyle = Normal;
        emit signal_moveCenter();
    }
    else if(e->button() == Qt::LeftButton)
    {
        lineEndPoint = e->pos();
        rulerIsPress = false;
    }
    setMouseTracking(true);
    updateMouseCursor(e->pos());
    update();
    QWidget::mouseReleaseEvent(e);
}

void PaintWidget::mouseMoveEvent (QMouseEvent *e)
{
    currentPos  = e->pos();
    if (checkstyle == RubberBand)
    {
        RubberEndPoint = e->pos();
        drawRubberRect();
        return;
    }
    else if (penStyle == Global::RulerStyle && rulerIsPress)
    {
        lineEndPoint = e->pos();
        endPos = e->pos();
        drawRuler();
        signal_mouseMove(e->pos());
        emit signal_updataDistance((lineEndPoint.x() - lineStartPoint.x())/(lineEndPoint.y() - lineStartPoint.y()) );
        emit signal_distancePoint(startPos, endPos);
        return;
    }
    else
    {
        updateMouseCursor(e->pos());
    }

    QWidget::mouseMoveEvent(e);
}

void PaintWidget::paintEvent (QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setRenderHint( QPainter::Antialiasing, true ); //Improve the quality of painting
    painter.drawPixmap(QPoint(0,0), Rubbelpix);
    QWidget::paintEvent(e);
}

void PaintWidget::resizeEvent (QResizeEvent *event)
{
    if (height () > pix->height () || width () > pix->width ())
    {
        QPixmap *newPix = new QPixmap(size());
        newPix->fill (Qt::transparent);
        QPainter p(newPix);
        p.drawPixmap (QPoint(0, 0), *pix);
        pix = newPix;
    }
    QWidget::resizeEvent (event);
}

void PaintWidget::setPaintStyle(Global::PaintStyle paintStyle)
{
    penStyle = paintStyle;
}

void PaintWidget::calcVertexes()
{
    double arrow_lenght_ = 13;//Arrow Len
    double arrow_degrees_ = 0.6;//Arrow angle

    double angle = atan2(lineEndPoint.y() - lineStartPoint.y(), lineEndPoint.x() - lineStartPoint.x()) + 3.1415926;//

    rightArrowPoint.setX(lineEndPoint.x() + arrow_lenght_ * cos(angle - arrow_degrees_));
    rightArrowPoint.setY(lineEndPoint.y() + arrow_lenght_ * qSin(angle - arrow_degrees_));
    leftArrowPoint.setX(lineEndPoint.x() + arrow_lenght_ * qCos(angle + arrow_degrees_));
    leftArrowPoint.setY(lineEndPoint.y() + arrow_lenght_ * qSin(angle + arrow_degrees_));
}

void PaintWidget::clear ()
{
    QPixmap *clearPix = new QPixmap(size());
    clearPix->fill (Qt::transparent);
    pix = clearPix;
    update ();
}

void PaintWidget::slot_setPaintStyle(Global::PaintStyle paintstyle)
{
    penStyle = paintstyle;
}

void PaintWidget::draw_point_text(double x, double y, QString Stringsize)
{
        *m_text_pix = *pix;
        m_is_drawtext = true;
        QPen pen;
        QPainter painter(m_text_pix);
        pen.setStyle ((Qt::PenStyle)style);
        pen.setWidth (weight);
        pen.setColor (color);
        painter.setPen(pen);
        Stringsize = "size: " + Stringsize;
        painter.drawText(x * width(), y * height() - 10, Stringsize);
        updateMouseCursor(currentPos);
}

void PaintWidget::updateMouseCursor(QPoint pos)
{
    if (m_is_drawtext)
    {
        Rubbelpix = *m_text_pix;
    }
    else
    {
        Rubbelpix = *pix;
    }
    QPainter painter(&Rubbelpix);
    QPen pen;
    pen.setWidth (weight);
    pen.setColor (color);
    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);
    painter.drawLine(pos.x(), 0, pos.x(), height());
    painter.drawLine(0, pos.y(), width(), pos.y());
    update();
}

void PaintWidget::drawRubberRect()
{
    if (m_is_drawtext)
    {
        Rubbelpix = *m_text_pix;
    }
    else
    {
        Rubbelpix = *pix;
    }
    QPen pen;
    pen.setColor(color);
    pen.setWidth(weight);
    pen.setStyle(Qt::DotLine);

    QPainter tempPainter(&Rubbelpix);
    tempPainter.setPen(pen);
    tempPainter.drawRoundRect(QRect(RubberStartPoint, RubberEndPoint), 0, 0);
    update();
}

void PaintWidget::drawRuler()
{
    calcVertexes();
    if (m_is_drawtext)
    {
        Rubbelpix = *m_text_pix;
    }
    else
    {
        Rubbelpix = *pix;
    }
    QPen pen;
    pen.setStyle ((Qt::PenStyle)style);
    pen.setWidth (weight);
    pen.setColor (color);
    QPainter tempPainter(&Rubbelpix);
    tempPainter.setRenderHint(QPainter::Antialiasing, true);
    tempPainter.setPen(pen);
    tempPainter.drawLine(lineStartPoint, lineEndPoint);
    tempPainter.drawLine(lineEndPoint, rightArrowPoint);
    tempPainter.drawLine(lineEndPoint, leftArrowPoint);
    update();
}

}
