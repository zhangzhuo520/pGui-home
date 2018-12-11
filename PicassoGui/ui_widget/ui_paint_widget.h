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

#include "ui_measurepoint.h"
#include "deftools/global.h"
#include "deftools/datastruct.h"


namespace ui {

class PaintPrivate
{
      explicit PaintPrivate(QWidget *parent = 0);

private:

};

class PaintWidget : public QWidget
{
    Q_OBJECT

    enum LineClicks
    {
        LineStart,
        LineEnd
    };

    enum MouseState{
        Press,
        release
    };

public:
    explicit PaintWidget(QWidget *parent = 0);

    void draw_defect_point_text(double, double, QString);

    QImage merge_two_images(const QImage& baseImage, const QImage&);

    void resizeRuler(double, double, double, double);

    void setPaintStyle(Global::PaintTool);
protected:
    virtual void mousePressEvent (QMouseEvent *);
    virtual void mouseReleaseEvent (QMouseEvent *);
    virtual void mouseMoveEvent (QMouseEvent *);
    virtual void paintEvent (QPaintEvent *);
    virtual void resizeEvent (QResizeEvent *);

signals:
    void signal_mouseMove(const QPoint&);

    void signal_updataDistance(double);

    void signal_moveCenter();

    void signal_get_snap_pos(QPoint, int);

public slots:
    void setStyle (int);
    void setWidth (QString);
    void setColor (QColor);
    void slot_get_snap_pos(QPoint, double, double, int);

    void clear();

private:
    void use_angle();
    void draw_cross_line(const QPoint&);
    void draw_dotted_box();
    void merge_image();
    void drawMeasureLine();
    QPointF get_render_frame_point(QPointF);

    Global::PaintTool m_select_mode;
    LineClicks m_mouse_clicks;
    MouseState m_mouse_state;

    QImage *m_empty_image;

    QImage m_ruler_image;
    QImage m_ruler_save_image;
    QImage m_cross_line_image;
    QImage m_dotted_box_image;
    QImage m_defectpoint_size_image;

    QImage m_paint_image;

    QPointF m_ruler_first_point;
    QPointF m_ruler_last_point;
    QPoint m_dotted_box_start;
    QPoint m_dotted_box_end;

    MeasureLine m_measure_point;

    QPoint m_current_mousepos;
    QPointF m_start_pos;
    QPointF m_end_pos;

    double m_scale_xstart;
    double m_scale_xend;
    double m_scale_ystart;
    double m_scale_yend;

    int weight;
    int style;
    QColor color;

    QString Stringsize;
    QString x;
    QString y;

    double distance;
};
}
#endif // DRAWWIDGET_H
