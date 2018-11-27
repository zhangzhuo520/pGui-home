#ifndef RENDER_FRAME_H
#define RENDER_FRAME_H
#include <vector>
#include <set>

#include <QMutex>
#include <QColor>
#include <QDebug>

#include "render_view_object.h"
#include "render_viewport.h"
#include "render_bitmap.h"
#include "render_view_op.h"
#include "render_pattern.h"
#include "render_line_style.h"
#include "render_layer_properties.h"

class QImage;
class QPixmap;

class QWheelEvent;
class QKeyEvent;
class QPaintEvent;
class QResizeEvent;
class QMouseEvent;

namespace Oasis
{
    class OasisLayout;

    class OasisBox;

}

namespace render{

class RenderFrame :public RenderObjectWidget
{
Q_OBJECT
public:
    RenderFrame(QWidget *parent = 0, QString path = "");

    virtual ~RenderFrame();

    void set_view_ops();

    QColor background_color() const { return QColor(m_background); }

    void set_pattern(const render::Pattern &p);

    const render::Pattern& pattern() const { return m_pattern; }

    Oasis::OasisLayout*  load_file(std::string file_name);

    const std::vector<render::LayerProperties>& get_properties_list() const;

    const render::LayerProperties& get_properties(int index) const;

    int layers_size() const
    {
        return m_layers_properties.size();
    }

    void set_properties(const render::LayerProperties& lp);

    void set_cursor_widget(QWidget *);

    Oasis::OasisBox get_box() const;

    void set_defect_point(double x, double y);

    void center_at_point(double x, double y);

    void zoom_center(int x, int y);

    virtual void paintEvent(QPaintEvent *);

    virtual void wheelEvent(QWheelEvent *);

signals:
    void signal_down_key_pressed();
    void signal_up_key_pressed();
    void signal_left_key_pressed();
    void signal_right_key_pressed();
    void signal_pos_updated(double x, double y);
    void signal_box_updated(double left, double bot, double right, double top);

public slots:
    void slot_down_shift();
    void slot_up_shift();
    void slot_left_shift();
    void slot_right_shift();
    void slot_distance_point(QPointF, QPointF);

protected:
    virtual void mouseMoveEvent(QMouseEvent* e);

    virtual void keyPressEvent(QKeyEvent* e);

    virtual void resizeEvent(QResizeEvent* e);

private:
    void init();

    void prepare_drawing();

    void zoom_box(const Oasis::OasisBox& box);

    void init_viewport();

    void shift_view(double scale, double dx, double dy);

    std::vector<render::ViewOp> m_view_ops;
    std::vector<render::Bitmap *> m_buffer;

    QImage *m_image;
    QPixmap *m_pixmap;
    double m_resolution;
    color_t m_background;

    QMutex m_mutex;
    render::Pattern m_pattern;
    render::LineStyle m_line_style;

    Oasis::OasisLayout* m_layout;
    render::Viewport m_vp;
    std::vector<render::LayerProperties> m_layers_properties;

    bool m_redraw_required;
    bool m_update_image;


};

}

#endif


