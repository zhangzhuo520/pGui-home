#include "drawwidget.h"
namespace UI {
DrawWidget::DrawWidget(QWidget *parent):
    QWidget(parent),
    weight(5),
    style(1),
    isDrawText(false),
    checkstyle(Normal),
    rulerIsPress(false)
{
    setAutoFillBackground (true);
    setMouseTracking(true);
    pix = new QPixmap(size());
    pix->fill (Qt::transparent);
    setColor(Qt::white);
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
            rulerIsPress = true;

        }
    }
     QWidget::mousePressEvent(e);
}

void DrawWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::RightButton)
    {
        RubberEndPoint = e->pos();
        checkstyle = Normal;
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

void DrawWidget::mouseMoveEvent (QMouseEvent *e)
{
    qDebug() << e->type();
    if (checkstyle == RubberBand)
    {
        RubberEndPoint = e->pos();
        drawRubberRect();
        return;
    }
    else if (penStyle == Global::RulerStyle && rulerIsPress)
    {
        lineEndPoint = e->pos();
        calcVertexes();
        drawRuler();
        signal_mouseMove(e->pos());
        emit signal_updataDistance((lineEndPoint.x() - lineStartPoint.x())/(lineEndPoint.y() - lineStartPoint.y()) );
        return;
    }
    else
    {
        updateMouseCursor(e->pos());
    }
    QWidget::mouseMoveEvent(e);
}

void DrawWidget::paintEvent (QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setRenderHint( QPainter::Antialiasing, true ); //Improve the quality of painting
    if(isDrawText)
    {
        isDrawText = false;
    }

    painter.drawPixmap(QPoint(0,0), Rubbelpix);
    e->accept();
}

void DrawWidget::resizeEvent (QResizeEvent *event)
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
    clearPix->fill (Qt::transparent);
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

        QPen pen;
        QPainter painter(pix);
        pen.setStyle ((Qt::PenStyle)style);
        pen.setWidth (weight);
        pen.setColor (color);
        painter.setPen(pen);
        QString spos = "X: " + x + "   ,Y: " + y;
        Stringsize = "SIZE: " + Stringsize;
        painter.drawText(width() / 2, height() / 2, Stringsize);
        painter.drawText(width() / 2, height() / 2 + 20, spos);
        update();
}

void DrawWidget::updateMouseCursor(QPoint pos)
{
    Rubbelpix = *pix;
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

void DrawWidget::drawRubberRect()
{
    Rubbelpix = *pix;
    QPen pen;
    pen.setColor(color);
    pen.setWidth(weight);
    pen.setStyle(Qt::DotLine);

    QPainter tempPainter(&Rubbelpix);
    tempPainter.setPen(pen);
    tempPainter.drawRoundRect(QRect(RubberStartPoint, RubberEndPoint), 0, 0);
    update();
}

void DrawWidget::drawRuler()
{
    Rubbelpix = *pix;
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
