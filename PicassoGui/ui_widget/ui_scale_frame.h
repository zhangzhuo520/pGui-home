#ifndef SCALEFRAME_H
#define SCALEFRAME_H

#include <QFrame>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QPoint>
#include <QLayout>
#include <QMouseEvent>
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

    void set_defect_point(double x, double y);

    void drawDefectPoint(double, double, QString);

    void calcu_defecttext_point();

    void draw_point_text();

    render::LayoutView load_file(const QString &, const QString &, bool);
    render::RenderFrame* getRenderFrame();

signals:
    void signal_pos_updated(double, double);
    void signal_updataDistance(double);
    void signal_box_updated();

public slots:
    void slot_box_updated(double,double,double,double);
    void slot_pos_updated(double, double);
    void slot_distance_updated(double);
    void slot_set_painter_style(Global::PaintStyle);
    void slot_set_pen_width(QString);
    void slot_set_pen_color(const QColor&);
    void slot_move_point_center();

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *e);

private:
    void initRenderFrame();
    void initImage();
    void paintImage();
    void setImageSize(QSize);
    void updateMouseCursor(QPoint);
    void darw_X_axis(QPainter &);
    void darw_Y_axis(QPainter &);
    double toDouble_1(double);

    double m_xratio;
    double m_yratio;
    double m_xratio_prev;
    double m_yratio_prev;
    double m_point_x;
    double m_point_y;
    QString m_size_text;
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
};
}
#endif // SCALEFRAME_H
