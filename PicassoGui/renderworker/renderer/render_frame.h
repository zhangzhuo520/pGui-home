#ifndef RENDER_FRAME_H
#define RENDER_FRAME_H
#include <vector>
#include <set>

#include <QMutex>
#include <QColor>

#include "render_view_object.h"
#include "render_viewport.h"
#include "render_bitmap.h"
#include "render_view_op.h"
#include "render_pattern.h"
#include "render_line_style.h"
#include "render_layer_properties.h"
#include "render_layout_view.h"

class QImage;
class QPixmap;
class QPoint;
class QLine;
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
    RenderFrame(QWidget *parent = 0);

    virtual ~RenderFrame();

    void set_view_ops();

    QColor background_color() const { return QColor(m_background); }

    void set_pattern(const render::Pattern &p);

    const render::Pattern& pattern() const { return m_pattern; }

    Oasis::OasisLayout*  load_file(std::string file_name, std::string prep_dir);

    render::LayoutView load_file(std::string file_name, std::string prep_dir, bool add_layout_view);

    void load_layout_view(render::LayoutView& lv, bool add_layout_view);

    const LayoutView& get_layout_view(int index) const;

    void set_layout_view(render::LayoutView& lv, int index);

    void create_and_initial_layer_properties(int lv_index, std::set<std::pair<int,int> >&layers);

    void update_view();

    void erase_layout_view(int index);

    void clear_layout_view();

    void delete_layer(int index);

    unsigned int current_layer() const
    {
        return m_current_layer;
    }

    void set_current_layer(unsigned int index)
    {
        m_current_layer = index;
    }

    int layout_views_size() const
    {
        return m_layout_views.size();
    }

    const std::vector<render::LayoutView>& get_layout_views_list() const
    {
        return m_layout_views;
    }

    const render::LayerProperties* get_properties(int index) const;

    size_t layers_size() const
    {
        return m_layers_properties.size();
    }

    void set_properties(unsigned int index , const render::LayerProperties& lp);

    void set_cursor_widget(QWidget *);

    Oasis::OasisBox get_box() const;

    const Oasis::OasisTrans& get_trans() const
    {
        return m_vp.trans();
    }


    std::pair<QPoint, std::pair<double,double> > get_snap_point(QPoint p);

    void set_defect_point(double x, double y);

    void center_at_point(double x, double y);

    void zoom_center(int x, int y);

    void add_layout_view(LayoutView& );

    void detach_layout_view(LayoutView& );

    virtual void paintEvent(QPaintEvent *);

    virtual void wheelEvent(QWheelEvent *);

signals:
    void signal_down_key_pressed();
    void signal_up_key_pressed();
    void signal_left_key_pressed();
    void signal_right_key_pressed();
    void signal_pos_updated(double x, double y);
    void signal_box_updated(double left, double bot, double right, double top);
    void signal_get_snap_pos(QPoint p, double, double, int mode);

public slots:
    void slot_down_shift();
    void slot_up_shift();
    void slot_left_shift();
    void slot_right_shift();
    void slot_get_snap_pos(QPoint, int mode);
    void slot_box_updated();
    void slot_zoom_in();
    void slot_zoom_out();

protected:
    virtual void mouseMoveEvent(QMouseEvent* e);

    virtual void keyPressEvent(QKeyEvent* e);

    virtual void resizeEvent(QResizeEvent* e);

private:
    void start_render();

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

    render::Viewport m_vp;
    std::vector<render::LayerProperties*> m_layers_properties;

    bool m_redraw_required;
    bool m_update_image;

    std::vector<render::LayoutView> m_layout_views;

    unsigned int m_current_layer;

};

}

#endif


