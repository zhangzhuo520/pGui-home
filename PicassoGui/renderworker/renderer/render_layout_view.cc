#include "render_layout_view.h"
#include "render_frame.h"

namespace render
{

LayoutView::LayoutView():m_widget(0), m_index(-1),m_file_name("")
{

}

LayoutView::LayoutView(int index, Oasis::OasisLayout *layout, render::RenderFrame* widget):m_widget(widget),m_layout(layout),m_index(index),m_file_name("")
{

}

LayoutView& LayoutView::operator=(const LayoutView & lv)
{
    if(& lv != this)
    {
        if(lv.m_widget != 0)
        {
            m_widget = lv.m_widget;
        }

        m_index = lv.m_index;
        m_layout = lv.m_layout;

        if(lv.m_file_name != "")
        {
            m_file_name = lv.m_file_name;
        }
    }
    return *this;
}

void LayoutView::append(LayoutView *view)
{
    if(view == this)
    {
        return ;
    }

    RenderFrame* old_widget = get_widget();

    if(old_widget)
    {
        old_widget->detach_layout_view(*this);
    }

    if(view->get_widget())
    {
        RenderFrame* frame = view->get_widget();
        frame->add_layout_view(*this);
    }
}

void LayoutView::detach()
{
    RenderFrame* frame = this->get_widget();
    frame->detach_layout_view(*this);
}

}
