#ifndef SCALEFRAME_H
#define SCALEFRAME_H

#include <QFrame>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QPoint>
#include <QLayout>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QDebug>
#include <math.h>

#include "ui_paint_widget.h"
#include "../renderer/render_frame.h"

namespace ui {

const double esp = 0.000001;
const int xSpace = 18;
const int ySpace = 18;
const int axis_unit_start = 18;
const int axis_unit_short = 15;
const int axis_unit_long = 5;


class ScaleFrame : public QFrame
{
    Q_OBJECT
public:
    explicit ScaleFrame(QWidget *parent = 0);

    void set_center_point(double x, double y, double view_range);
    void set_center_point(double x, double y);
    void set_defect_point(double x, double y);
    void drawDefectPoint(double, double, QString);
    void draw_gauge_line(QPointF, QPointF, QString);
    void get_canvas_coord(double *, double *, double *, double *);
    void calcu_defecttext_point();
    void draw_point_size();
    void repaint_image();
    void is_show_axis(bool);

    const QString &get_file_name() const
    {
        return m_filename;
    }
    void set_file_name(const QString& file_name)
    {
        m_filename = file_name;
    }

    void set_frame_info(const FrameInfo &frame_info)
    {
        m_frame_info = frame_info;
    }

    FrameInfo &frame_info()
    {
        return m_frame_info;
    }

    QVector<QString> get_file_name_list();
    double get_view_range() const;

    render::RenderFrame* getRenderFrame();
    const QList<LineData*> & get_measure_line_list();
    void set_measure_line_list(const QList<LineData*> &);

    const QList<LineData*> &get_cutline_list();
    void set_cutline_list(const QList<LineData*>&);

    void zoom_in();
    void zoom_out();
    void refresh();
    void zoom_fit();

signals:
    void signal_pos_updated(double, double);
    void signal_updateDistance(QString);
    void signal_box_updated();
    void signal_zoom_in();
    void signal_zoom_out();
    void signal_refresh();
    void signal_zoom_fit();
    void signal_update_measuretable();
    void signal_updata_cutline_table();
    void signal_layout_view_changed(render::RenderFrame*);

public slots:
    void slot_box_updated(double,double,double,double);
    void slot_pos_updated(double, double);
    void slot_distance_updated(QString);
    void slot_set_painter_style(Global::PaintTool);
    void slot_set_pen_width(QString);
    void slot_set_pen_color(const QColor&);
    void slot_move_point_center();
    void slot_set_snapfalg(Global::SnapFLag);
    void slot_clear_all();
    void slot_clear_measureline();
    void slot_clear_mark_point();
    void slot_clear_gauge();
    void slot_update_mesuretable();
    void slot_update_cutline_table();
    void slot_layout_view_changed(render::RenderFrame*);
    void slot_set_background_color(QColor);
    void slot_paint_cutline(Global::RtsCutLineAngle, QVariant);

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *e);

    void closeEvent(QCloseEvent* e);
private:
    void initRenderFrame();
    void initImage();
    void paintImage();
    void setImageSize(QSize);
    void updateMouseCursor(QPoint);
    void darw_X_axis(QPainter &);
    void darw_Y_axis(QPainter &);
    double toDouble_1(double);

    QString m_filename;
    double m_xratio;
    double m_yratio;

    bool m_isdraw_pointtext;
    QImage *m_image;
    QPixmap m_cursor_pixmap;
    double m_xstart;
    double m_xend;
    double m_ystart;
    double m_yend;
    int m_image_width;
    int m_image_heigth;
    double m_current_posX;
    double m_current_posY;


    QHBoxLayout *m_hlayout;
    PaintWidget *m_paint_widget;
    render::RenderFrame *m_render_frame;

    static int overlay_times;

    bool m_axis_falg;
    FrameInfo m_frame_info;
};
}
#endif // SCALEFRAME_H
