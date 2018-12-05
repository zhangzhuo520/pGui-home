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
#include <QEvent>
#include <qmath.h>
#include <QModelIndex>
#include "deftools/global.h"
#include "deftools/datastruct.h"

namespace ui {

class PaintPrivate
{
public:
    enum CheckStyle{
        RubberBand,
        Normal
    };

    enum PaintStyle
    {
        MouseStyle,
        PenStyle,
        RulerStyle
    };

    explicit PaintPrivate();
};

class PaintWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PaintWidget(QWidget *parent = 0);


    void mousePressEvent (QMouseEvent *);
    void mouseReleaseEvent (QMouseEvent *);
    void mouseMoveEvent (QMouseEvent *);
    void paintEvent (QPaintEvent *);
    void resizeEvent (QResizeEvent *);
    void setPaintStyle(Global::PaintStyle);

    void calcVertexes();

    void updateMouseCursor(QPoint);
    void drawRubberRect();
    void drawRuler();
signals:
    void signal_mouseMove(const QPoint&);

    void signal_updataDistance(double);

    void signal_distancePoint(QPointF, QPointF);

    void signal_moveCenter();

public slots:
    void setStyle (int);
    void setWidth (QString);
    void setColor (QColor);
    void clear();
    void draw_point_text(double, double, QString);
    void slot_setPaintStyle(Global::PaintStyle);

private:
    QPoint RubberStartPoint;
    QPoint RubberEndPoint;

    QPoint currentPos;
    QPixmap *pix;
    QPixmap Rubbelpix;
    QPixmap *m_text_pix;
    QPointF startPos;
    QPointF endPos;
    bool m_is_drawtext;
    int weight;
    int style;
    QColor color;
    Global::PaintStyle penStyle;

    CheckStyle checkstyle;
    QPointF lineStartPoint;
    QPointF lineEndPoint;
    QPointF rightArrowPoint;
    QPointF leftArrowPoint;
    bool rulerIsPress;
    QString Stringsize;
    QString x;
    QString y;
};
}
#endif // DRAWWIDGET_H
