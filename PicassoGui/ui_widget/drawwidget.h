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

public slots:
    void setStyle (int);
    void setWidth (QString);
    void setColor (QColor);
    void clear ();
    void drawPoint(const QModelIndex &);

    void slot_setPaintStyle(Global::PaintStyle);
private:    

    QPoint RubberStartPoint;
    QPoint RubberEndPoint;

    CheckStyle checkstyle;
    QPixmap *pix;
    QPixmap Rubbelpix;
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
    bool rulerIsPress;
    QString Stringsize;
    QString x;
    QString y;
    bool isDrawText;
};
}
#endif // DRAWWIDGET_H
