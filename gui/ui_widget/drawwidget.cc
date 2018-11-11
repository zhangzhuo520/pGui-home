#include "drawwidget.h"

DrawWidget::DrawWidget(QWidget *parent) : render::RenderFrame(parent)
{
    setAutoFillBackground (true);
    pix = new QPixmap(size());
    pix->fill (Qt::black);
    setColor(Qt::white);
    weight = 5;
    style =  1;
    isDrawText = false;
}

void DrawWidget::setStyle (int s)
{
    style = s;
}

void DrawWidget::setWidth (QString w)
{
    weight = w.toInt();
}

void DrawWidget::setColor (QColor c)
{
    color = c;
}

void DrawWidget::mousePressEvent (QMouseEvent *e)
{
    if (penStyle == Global::MouseStyle)
    {
      setCursor(Qt::CrossCursor);
    }
    else if (penStyle == Global::PenStyle)
    {
          startPos = e->pos ();
          setCursor(Qt::PointingHandCursor);
    }
    else if (penStyle == Global::RulerStyle)
    {
        setCursor(Qt::ArrowCursor);
        if (hasMouseTracking())
        {
            setMouseTracking(false);
        }
        else
        {
            lineStartPoint = lineEndPoint = e->pos();
            setMouseTracking(true);
            update();
        }
    }

}

void DrawWidget::mouseMoveEvent (QMouseEvent *e)
{
    if (penStyle == Global::MouseStyle)
    {

    }
    else if (penStyle == Global::PenStyle)
    {
        QPainter *painter = new QPainter;
        QPen pen;
        pen.setStyle ((Qt::PenStyle)style);
        pen.setWidth (weight);
        pen.setColor (color);
        painter->begin (pix);
        painter->setPen (pen);
        painter->drawLine (startPos, e->pos ());
        painter->end ();
        startPos = e->pos ();
    }
    else if (penStyle == Global::RulerStyle)
    {
        lineEndPoint = e->pos();
        calcVertexes();
    }

    update ();
    signal_mouseMove(e->pos());
}

void DrawWidget::paintEvent (QPaintEvent *e)
{
    Q_UNUSED(e);
    render::RenderFrame::paintEvent(e);
/*
    QPainter painter(this);
    painter.setRenderHint( QPainter::Antialiasing, true ); //Improve the quality of painting
    painter.drawPixmap (QPoint(0,0), *pix);
    if (penStyle == Global::MouseStyle)
    {

    }
    else if (penStyle == Global::PenStyle)
    {

    }
    else if (penStyle == Global::RulerStyle)
    {
        QPen pen;
        pen.setStyle ((Qt::PenStyle)style);
        pen.setWidth (weight);
        pen.setColor (color);
        painter.setPen(pen);
        painter.drawLine(lineStartPoint, lineEndPoint);
        painter.drawLine(lineEndPoint, rightArrowPoint);
        painter.drawLine(lineEndPoint, leftArrowPoint);
        emit signal_updataDistance((lineEndPoint.x() - lineStartPoint.x())/(lineEndPoint.y() - lineStartPoint.y()) );
    }

    if(isDrawText)
    {
        QPen pen;
        pen.setStyle ((Qt::PenStyle)style);
        pen.setWidth (weight);
        pen.setColor (color);
        painter.setPen(pen);
        QString spos = "X: " + x + "   ,Y: " + y;
        Stringsize = "SIZE: " + Stringsize;
        painter.drawText(width() / 2, height() / 2, Stringsize);
        painter.drawText(width() / 2, height() / 2 + 20, spos);
        isDrawText = false;
    }
*/
}

void DrawWidget::resizeEvent (QResizeEvent *event)
{
    if (height () > pix->height () || width () > pix->width ())
    {
        QPixmap *newPix = new QPixmap(size());
        newPix->fill (Qt::black);
        QPainter p(newPix);
        p.drawPixmap (QPoint(0, 0), *pix);
        pix = newPix;
    }
    QWidget::resizeEvent (event);
}

void DrawWidget::calcVertexes()
{
    double arrow_lenght_ = 13;//Arrow Len
    double arrow_degrees_ = 0.6;//Arrow angle

    double angle = atan2(lineEndPoint.y() - lineStartPoint.y(), lineEndPoint.x() - lineStartPoint.x()) + 3.1415926;//

    rightArrowPoint.setX(lineEndPoint.x() + arrow_lenght_ * cos(angle - arrow_degrees_));
    rightArrowPoint.setY(lineEndPoint.y() + arrow_lenght_ * qSin(angle - arrow_degrees_));
    leftArrowPoint.setX(lineEndPoint.x() + arrow_lenght_ * qCos(angle + arrow_degrees_));
    leftArrowPoint.setY(lineEndPoint.y() + arrow_lenght_ * qSin(angle + arrow_degrees_));
}

void DrawWidget::clear ()
{
    QPixmap *clearPix = new QPixmap(size());
    clearPix->fill (Qt::black);
    pix = clearPix;
    update ();
}

void DrawWidget::slot_setPaintStyle(Global::PaintStyle paintstyle)
{
    penStyle = paintstyle;
}

void DrawWidget::drawPoint(const QModelIndex & index)
{
        isDrawText = true;
        Stringsize = index.sibling(index.row(), 1).data().toString();
        x = index.sibling(index.row(), 2).data().toString();
        y = index.sibling(index.row(), 3).data().toString();
        update();
}
