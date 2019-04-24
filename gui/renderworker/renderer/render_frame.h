
#ifndef RENDER_FRAME_H
#define RENDER_FRAME_H
#include <vector>
#include <set>

#include <QMutex>
#include <QColor>
#include <QTime>
#include "oasis_types.h"

#include "render_view_object.h"
#include "render_viewport.h"
#include "render_view_op.h"
#include "render_pattern.h"
#include "render_line_style.h"
#include "render_layout_view.h"

class QImage;
class QPixmap;
class QPoint;
class QPointF;
class QLine;
class QLineF;
class QWheelEvent;
class QKeyEvent;
class QPaintEvent;
class QResizeEvent;
class QMouseEvent;

namespace oasis
{
    class OasisLayout;
}

namespace render{

class RenderManager;
class BitmapManager;
class Bitmap;
class LayerProperties;
class RTSImage;

class RenderFrame :public RenderObjectWidget
{
Q_OBJECT
public:

    enum MouseStyle
    {
        Normal,
        Busy
    };

    RenderFrame(QWidget *parent = 0);

    virtual ~RenderFrame();

    void set_view_ops();

    QColor background_color() const { return QColor(m_bg_color); }

    void set_background_color(QColor color);

    void set_pattern(const render::Pattern &p);

    const render::Pattern& pattern() const { return m_pattern; }

    void load_layout_view(render::LayoutView* lv, std::string prep_dor, bool add_layout_view);

    void add_layout_view(render::LayoutView* lv, bool add_layout_view);

    LayoutView* get_layout_view(int index);

    void set_layout_view(render::LayoutView* lv, int index);

    void create_and_initial_layer_properties(int lv_index, std::set<std::pair<int,int> >&layers);

    void update_view_ops();

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

    size_t layers_size() const
    {
        return m_layers_properties.size();
    }

    const oasis::OasisTrans& get_trans() const
    {
        return m_vp.trans();
    }

    int worker_nums() const
    {
        return m_worker_nums;
    }

    std::vector<render::LayoutView*> get_layout_views_list() const;

    const render::LayerProperties* get_properties(int index) const;

    void set_layer_cached(int index, bool cache);

    int tree_to_list_index(int parent_index, int child_index);

    void list_to_tree_index(int index, int& parent_index, int& child_index);

    void set_properties(unsigned int index , const render::LayerProperties& lp);

    void set_cursor_widget(QWidget *);

    oasis::BoxF get_box() const;

    std::pair<bool, std::pair<QPointF, QPointF> > get_snap_point(QPointF p, int mode);

    std::pair<bool, std::pair<QLineF, QLineF> >get_snap_edge(QPointF p, int mode);

    void set_defect_point(double x, double y);

    void set_center_point(double x, double y, double view_range = 10.0);

    void center_at_point(double x, double y);

    void zoom_center(double x, double y);

    void add_layout_view(LayoutView* );

    void detach_layout_view(LayoutView* );

    virtual void paintEvent(QPaintEvent *);

    virtual void wheelEvent(QWheelEvent *);

    int index_of_layout_views(LayoutView* lv);
    Bitmap* create_bitmap();
    void update_image();

    void finish_drawing();
    void init_bitmaps(unsigned int layers, unsigned int width,
                     unsigned int height, double resolution,
                     bool shift, const oasis::Point&shift_disp,
                     bool restart, const std::vector<int>& restart_bitmaps);

    void set_worker_nums(int workers);
    double get_view_range() const;

    virtual QImage& background_image() const
    {
        return *m_image;
    }

    std::vector<QString> get_rts_image_paths();
    void disable_rts_image(QString& file_path);
    void enable_rts_image(QString& file_path);
    void disble_all_rts_image();
    void clear_all_rts_images();

    void add_rts_image(const QStringList& info_path_list);
    void set_mouse_style(MouseStyle style);

    void copy_bitmaps(const std::vector<int>& update_layers, const std::vector<render::Bitmap*>& planes, int x, int y);
    void copy_bitmap(int update_layers, render::Bitmap* contour_bitmap, render::Bitmap* fill_bitmap, int x, int y);

signals:
    void signal_down_key_pressed();
    void signal_up_key_pressed();
    void signal_left_key_pressed();
    void signal_right_key_pressed();
    void signal_zoom_out_pressed();
    void signal_zoom_in_pressed();
    void signal_pos_updated(double x, double y);
    void signal_box_updated(double left, double bot, double right, double top);
    void signal_get_snap_pos(bool, QPointF, QPointF, int mode);
    void signal_get_snap_edge(bool, QLineF, QLineF, int);
    void signal_layout_view_changed(render::RenderFrame*);

public slots:
    void slot_down_shift();
    void slot_up_shift();
    void slot_left_shift();
    void slot_right_shift();
    void slot_get_snap_pos(QPointF, int mode);
    void slot_get_snap_edge(QPointF, int mode);
    void slot_box_updated();
    void slot_zoom_in();
    void slot_zoom_out();
    void slot_refresh();
    void slot_zoom_fit();
    void slot_go_to_position(QPointF);

protected:
    virtual void mouseMoveEvent(QMouseEvent* e);

    virtual void keyPressEvent(QKeyEvent* e);

    virtual void resizeEvent(QResizeEvent* e);

private:
    void start_render();

    void prepare_drawing();

    void zoom_box(const oasis::BoxF& box);

    void init_viewport();

    void shift_view(double scale, double dx, double dy);

    void redraw_all();
    void stop_redraw();

    std::vector<RTSImage*> get_rts_images();

    std::vector<render::ViewOp> m_view_ops;
    std::vector<render::Bitmap *> m_buffer;
    std::vector<render::Bitmap *> m_split_bitmaps;

    QImage *m_image;
    QImage *m_image_bg;
    QPixmap *m_pixmap;
    double m_resolution;

    QMutex m_mutex;
    render::Pattern m_pattern;
    render::LineStyle m_line_style;

    render::Viewport m_vp;
    std::vector<render::LayerProperties*> m_layers_properties;

    bool m_redraw_required;
    bool m_update_image;

    std::vector<render::LayoutViewProxy> m_layout_views;

    unsigned int m_current_layer;

    color_t m_bg_color;

    BitmapManager* m_bitmap_manager;
    RenderManager* m_render_manager;

    bool m_restart_render;
    int m_worker_nums;

    std::vector<int> m_required_redraw_layer;
    QTime m_draw_timer;
};
}
#endif


