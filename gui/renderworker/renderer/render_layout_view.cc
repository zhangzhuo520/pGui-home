#include "render_layout_view.h"
#include "render_frame.h"

namespace render
{

LayoutView::LayoutView(): m_file_name(""),m_valid(false)
{

}

LayoutView::LayoutView(oasis::OasisLayout *layout):m_layout(layout),m_file_name(""),m_valid(false)
{

}


LayoutView& LayoutView::operator=(const LayoutView & lv)
{
    if(& lv != this)
    {
        m_layout = lv.m_layout;

        if(lv.m_file_name != "")
        {
            m_file_name = lv.m_file_name;
        }
    }
    return *this;
}

bool LayoutView::operator==(const LayoutView& lv)
{
    if(lv.get_layout() != get_layout())
    {
        return false;
    }
    else
    {
        return true;
    }
}

void LayoutView::load_into_frame(render::RenderFrame* frame, std::string prep_dir)
{
    if(frame == 0)
    {
        return ;
    }

    frame->load_layout_view(this, prep_dir, false);
    set_valid(true);
}

bool LayoutView::has_single_view()
{
    bool result = false;
    if(single_view())
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}

render::RenderFrame* LayoutView::single_view()
{
    render::RenderFrame* frame = 0;
    for(size_t i = 0; i < m_proxys.size(); i++)
    {
        render::RenderFrame* tmp = m_proxys[i].frame();
        if(tmp->layout_views_size() == 1 && tmp->get_layout_view(0) == this)
        {
            frame = tmp;
            break;
        }
    }

    return frame;
}

static bool equal(const std::vector<LayoutView*>& a, const std::vector<LayoutView*>& b)
{
    if(a.size() != b.size())
    {
        return false;
    }

    bool result = true;
    for(size_t j = 0; j < a.size(); j++)
    {
        if(a[j] != b[j])
        {
            result = false;
            break;
        }
    }

    return result;
}

void LayoutView::attach(render::RenderFrame* frame, std::string prep_dir, bool add_layout_view)
{
    if(frame == 0)
    {
        return;
    }

    std::vector<LayoutView* > append_result;
    append_result.push_back(this);

    for(int i = 0; i < frame->layout_views_size(); i++)
    {
        if(frame->get_layout_view(i) == this)
        {
            throw append_error("The selected frame has included the file.");
        }
        else
        {
            append_result.push_back(frame->get_layout_view(i));
        }
    }

    std::sort(append_result.begin(), append_result.end());

    for(size_t i = 0; i < m_proxys.size(); i++)
    {
        std::vector<LayoutView*> local_result;
        render::RenderFrame* frame = m_proxys[i].frame();
        for(int j = 0; j < frame->layout_views_size(); j++)
        {
            local_result.push_back(frame->get_layout_view(j));
        }

        std::sort(local_result.begin(), local_result.end());

        if(equal(local_result, append_result))
        {
            throw append_exception("The append result has been shown", frame);
        }
    }

    if(valid())
    {
        frame->add_layout_view(this, add_layout_view);
    }
    else
    {
        frame->load_layout_view(this, prep_dir, add_layout_view);
        set_valid(true);
    }
}

void LayoutView::close()
{
    std::vector<render::RenderFrame*> frame_list;
    for(size_t i = 0; i < m_proxys.size(); i++)
    {
        frame_list.push_back(m_proxys[i].frame());
    }

    for(size_t i = 0; i < frame_list.size(); i++)
    {
        frame_list[i]->detach_layout_view(this);
    }


}

void LayoutView::erase_proxy(LayoutViewProxy& lv)
{
    for(size_t i = 0 ; i < m_proxys.size(); i++)
    {
        if(m_proxys[i] == lv)
        {
            m_proxys.erase(m_proxys.begin() + i);
            return;
        }

    }
}

LayoutViewProxy::LayoutViewProxy(LayoutView* view, RenderFrame* frame)
{
    m_layout_view = view;
    m_frame = frame;
}

bool LayoutViewProxy::operator ==(const LayoutView* lv) const
{
    if(!is_valid())
    {
        return false;
    }
    else
    {
        return m_layout_view == lv;
    }
}

bool LayoutViewProxy::is_valid() const
{
    return m_layout_view->valid() && m_frame;
}

int LayoutViewProxy::index() const
{
    if(!is_valid())
    {
        return -1;
    }
    else
    {
        return m_frame->index_of_layout_views(m_layout_view);
    }
}

render::RenderFrame* LayoutViewProxy::frame()
{
    return m_frame;
}

LayoutView* LayoutViewProxy::operator->() const
{
    return m_layout_view;
}

}
