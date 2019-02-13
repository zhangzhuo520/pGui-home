#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QColor>
#include <QPixmap>
#include <QPair>
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

public:
    explicit PaintWidget(QWidget *parent = 0);

    void draw_defect_point_text(double, double, QString);
    void draw_gauge_line(QPointF, QPointF, QString);
    void clear_gauge_line();
    void setPaintStyle(Global::PaintTool);
    void set_snap_flag(Global::SnapFLag);

    void set_measure_line_list(const QList <LineData>&);
    const QList <LineData>& get_measure_line_list();

    void repaint_image(double, double, double, double);



protected:
    virtual void mousePressEvent (QMouseEvent *);
    virtual void mouseReleaseEvent (QMouseEvent *);
    virtual void mouseMoveEvent (QMouseEvent *);
    virtual void paintEvent (QPaintEvent *);
    virtual void resizeEvent (QResizeEvent *);
    virtual void keyPressEvent(QKeyEvent *);

signals:
    void signal_mouseMove(const QPoint&);
    void signal_updateDistance(double);
    void signal_moveCenter();
    void signal_get_snap_pos(QPointF, int);
    void signal_measure_line_list();

public slots:
    void setStyle (int);
    void setWidth (QString);
    void setColor (QColor);
    void slot_get_snap_pos(bool, QPointF, QPointF, int);
    void slot_clear_all();
    void slot_measure_line_clear();
    void slot_mark_point();
    void slot_box_updated(double, double, double, double);

private:
    QPointF calcu_physical_point(QPointF);
    QPointF calcu_pixel_point(QPointF);

    void use_angle();
    void draw_cross_line(const QPoint&);
    void draw_dotted_box();
    void draw_measure_line();
    void draw_mark_cross();

    QImage merge_two_images(const QImage&, const QImage&);
    void merge_image();

    void repaint_normal_ruler();
    void repaint_mark_cross();
    void repaint_defect_point();
    void repaint_gauge_line();

    void process_snap_point(bool find, QPointF& pixel_point, QPointF& micron_point, bool(*func)(QPointF, QPointF));
    Global::PaintTool m_select_mode;
    LineClicks m_mouse_clicks;
    Global::SnapFLag m_snap_flag;

    QImage *m_empty_image;

    QImage m_ruler_image;
    QImage m_ruler_save_image;
    QImage m_cross_line_image;
    QImage m_dotted_box_image;
    QImage m_defectpoint_size_image;
    QImage m_mark_cross_image;
    QImage m_gauge_image;

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

    bool m_isdraw_defect_points;
    QString m_defect_size;
    double m_defect_x;
    double m_defect_y;

    double m_distance;

    bool m_first_point_find;

    QList <QPointF> m_mark_cross_list;
    QPointF m_gauge_start;
    QPointF m_gauge_end;
    QString m_gauge_info;
};
}
#endif // DRAWWIDGET_H
