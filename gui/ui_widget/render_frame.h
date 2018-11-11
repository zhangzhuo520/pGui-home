#ifndef RENDER_FRAME_H
#define RENDER_FRAME_H
#include <vector>

#include <QWidget>
#include <QFrame>
#include <QMutex>
#include <QPaintEvent>
#include <QColor>
#include "render_bitmap.h"
#include "render_view_op.h"
#include "render_pattern.h"
#include "render_line_style.h"

class QImage;
class QPixmap;

namespace render{

class RenderFrame : public QFrame
{
public:
    RenderFrame(QWidget *parent = 0);
    ~RenderFrame();
    virtual void paintEvent(QPaintEvent *);
    void set_view_ops();

    QColor background_color() const { return QColor(m_background); }

    void set_pattern(const render::Pattern &p);

    const render::Pattern& pattern() const { return m_pattern; }

private:
    void init();
    void prepare_drawing();

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

};

}

#endif


