#include "render_view_object.h"
#include "render_bitmap.h"

namespace render
{
RenderObject::RenderObject(RenderObjectWidget* _widget, bool is_static): m_view(_widget),m_visible(true),m_static(is_static)
{
    if(_widget)
    {
        widget()->m_objects.push_back(this);
    }
}

void RenderObject::set_visible(bool visible)
{
    m_visible = visible;
    redraw();
}


void RenderObject::redraw()
{
    if(widget())
    {
        if(m_static)
        {
            widget()->update_static_foreground();
        }
        else
        {
            widget()->update();
        }
    }
}

RenderObjectWidget::RenderObjectWidget(QWidget * parent):QWidget(parent),m_plane_width(0), m_plane_height(0), m_plane_resolution(0),m_require_update_static(false)
{

}

void RenderObjectWidget::update_static_foreground()
{
    if(!m_require_update_static)
    {
        m_require_update_static = true;
        update();
    }
}

void RenderObjectWidget::clear_foreground()
{
    while(!m_foreground_bitmaps.empty())
    {
        if(m_foreground_bitmaps.back())
        {
            delete m_foreground_bitmaps.back();
        }
        m_foreground_bitmaps.pop_back();
        m_foreground_view_ops.pop_back();
    }
    m_foreground_bitmaps.clear();
    m_foreground_view_ops.clear();
    m_foreground_table.clear();
}

RenderObjectWidget::~RenderObjectWidget()
{
    clear_foreground();
    for(std::vector<render::RenderObject* >::iterator it = m_objects.begin(); it != m_objects.end(); it++)
    {
        delete (*it);
    }
}

void RenderObjectWidget::render_foreground(const Viewport &viewport, RenderObjectWidget *frame, bool is_static)
{
    if(is_static)
    {
        m_require_update_static = false;
    }
    for(std::vector<render::RenderObject *>::iterator it = m_objects.begin(); it != m_objects.end(); it++)
    {
        if((*it)->is_visible() && (*it)->m_static == is_static)
        (*it)->render(viewport, frame);
    }
}

render::Bitmap* RenderObjectWidget::planes(const render::ViewOp &vp)
{
    std::map<render::ViewOp, int>::iterator it = m_foreground_table.find(vp);
    if(it == m_foreground_table.end())
    {
        m_foreground_table.insert(std::make_pair(vp, m_foreground_bitmaps.size()));
        render::Bitmap* bitmap = new render::Bitmap(m_plane_width, m_plane_height, m_plane_resolution);
        m_foreground_bitmaps.push_back(bitmap);
        m_foreground_view_ops.push_back(vp);
        return bitmap;
    }
    else
    {
        return m_foreground_bitmaps[it->second];
    }
    sort_planes();
}

namespace
{
    struct pair_compare
    {
        bool operator()(std::pair<render::ViewOp, render::Bitmap*> a, std::pair<render::ViewOp, render::Bitmap*> b)
        {
            return a.first < b.first;
        }
    };
}


void RenderObjectWidget::sort_planes()
{
    std::vector<std::pair<render::ViewOp, render::Bitmap* > > result;
    for(unsigned int i = 0; i < m_foreground_bitmaps.size(); i++)
    {
        result.push_back(std::make_pair(m_foreground_view_ops[i], m_foreground_bitmaps[i]));
    }
    std::sort(result.begin(), result.end(), pair_compare());
    for(unsigned int i = 0; i < m_foreground_bitmaps.size(); i++)
    {
        m_foreground_view_ops[i] = result[i].first;
        m_foreground_bitmaps[i] = result[i].second;
    }

}

}
