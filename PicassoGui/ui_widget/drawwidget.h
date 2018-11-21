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

namespace UI {
class DrawWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DrawWidget(QWidget *parent = 0);

    enum CheckStyle{
        RubberBand,
        Normal
    };
    void mousePressEvent (QMouseEvent *);
    void mouseReleaseEvent (QMouseEvent *);
    void mouseMoveEvent (QMouseEvent *);
    void paintEvent (QPaintEvent *);
    void resizeEvent (QResizeEvent *);

    void calcVertexes();

    void updateMouseCursor(QPoint);
    void drawRubberRect();
    void drawRuler();
signals:
    void signal_mouseMove(const QPoint&);

    void signal_updataDistance(double);

    void signal_distancePoint(QPointF, QPointF);

public slots:
    void setStyle (int);
    void setWidth (QString);
    void setColor (QColor);
    void clear ();
    void drawPoint(const QModelIndex &);
    void slot_updataPos(double, double);

    void slot_setPaintStyle(Global::PaintStyle);
private:    
    QPoint RubberStartPoint;
    QPoint RubberEndPoint;

    QPointF currentPos;
    QPixmap *pix;
    QPixmap Rubbelpix;
    QPointF startPos;
    QPointF endPos;
    int weight;
    int style;
    QColor color;
    Global::PaintStyle penStyle;

    bool isDrawText;
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
