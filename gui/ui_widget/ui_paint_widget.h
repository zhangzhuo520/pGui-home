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
    void set_paint_style(Global::PaintTool);
    void set_snap_flag(Global::SnapFLag);
    void set_rts_cutline(Global::RtsCutLineAngle, QVariant);

    void set_measure_line_list(const QList <LineData*>&);
    const QList <LineData*>& get_measure_line_list();

    void set_cutline_list(const QList <LineData*>&);
    const QList <LineData*>& get_cutline_list();
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
    void signal_updateDistance(QString);
    void signal_moveCenter();
    void signal_get_snap_pos(QPointF, int);
    void signal_get_snap_edge(QPointF, int);
    void signal_measure_line_list();
    void signal_cutline_list();
    void signal_go_to_position(QPointF);

public slots:
    void setStyle (int);
    void setWidth (QString);
    void setColor (QColor);
    void slot_get_snap_pos(bool, QPointF, QPointF, int);
    void slot_get_snap_edge(bool find, QLineF pix_edge, QLineF micron_edge, int mode);
    void slot_clear_all();
    void slot_measure_line_clear();
    void slot_mark_point();
    void slot_box_updated(double, double, double, double);

private:
    QPointF calcu_physical_point(QPointF);
    QPointF calcu_pixel_point(QPointF);

    void use_angle();
    void rts_hvd_angle();
    void rts_hv_angle();
    void rts_user_input_angle();
    void rts_user_input_coord();
    void draw_cross_line(const QPoint&);
    void draw_dotted_box();
    void draw_measure_line();
    void draw_mark_cross();
    void draw_cutline();
    void selet_rts_cutline_mode();
    void draw_base_line();

    QImage merge_two_images(const QImage&, const QImage&);
    void merge_image();

    void repaint_ruler();
    void repaint_mark_cross();
    void repaint_defect_point();
    void repaint_gauge_line();
    void repaint_cutline();

    void process_snap_point(bool find, QPointF& pixel_point, QPointF& micron_point, bool(*func)(QPointF, QPointF));
    Global::PaintTool m_select_mode;
    LineClicks m_mouse_clicks;
    Global::SnapFLag m_snap_flag;
    Global::RtsCutLineAngle m_cutline_mode;
    QVariant m_rts_cutline_data;

    QImage *m_empty_image;

    QImage m_ruler_image;
    QImage m_ruler_save_image;
    QImage m_cross_line_image;
    QImage m_dotted_box_image;
    QImage m_defectpoint_size_image;
    QImage m_mark_cross_image;
    QImage m_gauge_image;
    QImage m_cut_line_image;
    QImage m_cut_line_save_image;

    QImage m_paint_image;

    QPointF m_ruler_first_point;
    QPointF m_ruler_last_point;
    QPointF m_ruler_start_pos;
    QPointF m_ruler_end_pos;

    QPointF m_cutline_first_point;
    QPointF m_cutline_last_point;
    QPointF m_cutline_start_pos;
    QPointF m_cutline_end_pos;

    QPoint m_dotted_box_start;
    QPoint m_dotted_box_end;

    MeasureLine m_cutline_point;
    MeasureLine m_measure_line;
    QPoint m_current_mousepos;

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

    QString m_line_result;

    bool m_first_point_find;

    QList <QPointF> m_mark_cross_list;
    QPointF m_gauge_start;
    QPointF m_gauge_end;
    QString m_gauge_info;

    // oblique_line_group
    QLineF m_micron_e1;
    QLineF m_pixel_e1;

    QLineF m_micron_e2;
    QLineF m_pixel_e2;


};
}
#endif // DRAWWIDGET_H
