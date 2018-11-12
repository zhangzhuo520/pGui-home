#ifndef RENDER_FRAME_H
#define RENDER_FRAME_H
#include <vector>
#include <set>

#include <QWidget>
#include <QFrame>
#include <QMutex>
#include <QPaintEvent>
#include <QColor>
#include <QDebug>
#include <QMouseEvent>

#include "render_viewport.h"
#include "render_bitmap.h"
#include "render_view_op.h"
#include "render_pattern.h"
#include "render_line_style.h"
#include "render_layer_properties.h"

class QImage;
class QPixmap;

namespace Oasis
{
    class OasisLayout;
    
    class OasisBox;
}

namespace render{

class RenderFrame : public QFrame
{
Q_OBJECT
public:
    RenderFrame(QWidget *parent = 0);

    ~RenderFrame();

    virtual void paintEvent(QPaintEvent *);

    virtual void wheelEvent(QWheelEvent *);

    void set_view_ops();

    QColor background_color() const { return QColor(m_background); }

    void set_pattern(const render::Pattern &p);

    const render::Pattern& pattern() const { return m_pattern; }

    Oasis::OasisLayout*  load_file(std::string file_name);

    const std::vector<render::LayerProperties>& get_properties_list() const;

    void set_properties(const render::LayerProperties& lp);

    void set_cursor_widget(QWidget *);
protected:
    void mouseMoveEvent(QMouseEvent *e)
    {
        QWidget::mouseMoveEvent(e);
    }

private:
    void init();

    void prepare_drawing();

    void zoom_box(const Oasis::OasisBox& box);

    void set_viewport();

private:
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
};

}

#endif


