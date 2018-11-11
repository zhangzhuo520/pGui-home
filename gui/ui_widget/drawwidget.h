#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QColor>
#include <QPixmap>
#include <QPoint>
#include <QPainter>
#include <QPalette>
#include <QDebug>
#include <qmath.h>
#include <QModelIndex>
#include "deftools/global.h"
#include "deftools/datastruct.h"
#include "render_frame.h"

class DrawWidget : public render::RenderFrame
{
    Q_OBJECT
public:
    explicit DrawWidget(QWidget *parent = 0);
    void mousePressEvent (QMouseEvent *);
    void mouseMoveEvent (QMouseEvent *);
    void paintEvent (QPaintEvent *);
    void resizeEvent (QResizeEvent *);

    void calcVertexes();

signals:
    void signal_mouseMove(const QPoint&);

    void signal_updataDistance(double);

public slots:
    void setStyle (int);
    void setWidth (QString);
    void setColor (QColor);
    void clear ();
    void slot_setPaintStyle(Global::PaintStyle);
    void drawPoint(const QModelIndex &);
private:
    QPixmap *pix;
    QPoint startPos;
    QPoint endPos;
    int style;
    int weight;
    QColor color;
    Global::PaintStyle penStyle;

    QPointF lineStartPoint;
    QPointF lineEndPoint;
    QPointF rightArrowPoint;
    QPointF leftArrowPoint;
    QString Stringsize;
    QString x;
    QString y;
    bool isDrawText;
};

#endif // DRAWWIDGET_H
