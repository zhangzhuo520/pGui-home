#ifndef RENDER_VIEW_OBJECT_H
#define RENDER_VIEW_OBJECT_H

#include <vector>
#include <map>

#include <QWidget>

#include "render_view_op.h"

namespace render{

class Viewport;
class RenderFrame;
class Bitmap;
class RenderObjectWidget;

class RenderObject{
public:
    RenderObject(RenderObjectWidget* widget = 0, bool is_static = true);

    virtual void render(const render::Viewport& viewport, RenderObjectWidget* frame) = 0;

    bool is_visible() const
    {
        return m_visible;
    }

    void set_visible(bool visible);

    RenderObjectWidget* widget() const
    {
        return m_view;
    }

    void redraw();

private:
    friend class RenderObjectWidget;

    RenderObjectWidget* m_view;
    RenderObject(const RenderObject& r);
    RenderObject& operator =(const RenderObject& r);

    bool m_visible;
    bool m_static;
};

//Interface for Render Object container
class RenderObjectWidget :public QWidget
{
public:

    typedef std::vector<RenderObject*>::iterator object_iter;

    RenderObjectWidget(QWidget * parent);

    virtual ~RenderObjectWidget();

    void render_foreground(const render::Viewport& viewport, RenderObjectWidget* frame, bool is_static);

    void clear_foreground();

    std::size_t foreground_bitmaps() const
    {
        return m_foreground_bitmaps.size();
    }


    unsigned int plane_width() const
    {
        return m_plane_width;
    }


    void set_plane_width(unsigned int width)
    {
        m_plane_width = width;
    }


    unsigned int plane_height() const
    {
        return m_plane_height;
    }


    void set_plane_height(unsigned int height)
    {
        m_plane_height = height;
    }

    double plane_resolution() const
    {
        return m_plane_resolution;
    }

    void set_plane_resolution(double resolution)
    {
        m_plane_resolution = resolution;
    }

    render::Bitmap* planes(const render::ViewOp& vp);

    std::vector<render::Bitmap *> foreground_bitmaps_vector() const
    {
        return m_foreground_bitmaps;
    }

    std::vector<render::ViewOp> foreground_view_ops() const
    {
        return m_foreground_view_ops;
    }

    bool require_update_static() const
    {
        return m_require_update_static;
    }

    object_iter begin_object()
    {
        return m_objects.begin();
    }

    object_iter end_object()
    {
        return m_objects.end();
    }

private:

    friend class RenderObject;

    unsigned int m_plane_width;
    unsigned int m_plane_height;
    double m_plane_resolution;

    bool m_require_update_static;

    std::vector<render::RenderObject*> m_objects;
    std::map<render::ViewOp, int > m_foreground_table;
    std::vector<render::Bitmap*> m_foreground_bitmaps;
    std::vector<render::ViewOp> m_foreground_view_ops;

    void sort_planes();
    void update_static_foreground();
};

}
#endif // RENDER_VIEW_OBJECT_H
