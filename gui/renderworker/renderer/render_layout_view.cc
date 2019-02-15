#include "render_layout_view.h"
#include "render_frame.h"

namespace render
{

LayoutView::LayoutView():m_widget(0), m_index(-1),m_file_name(""),m_valid(false),m_enable_attach(true)
{

}

LayoutView::LayoutView(int index, oasis::OasisLayout *layout, render::RenderFrame* widget):m_widget(widget),m_layout(layout),m_index(index),m_file_name(""),m_valid(false),m_enable_attach(true)
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

void LayoutView:: attach(render::RenderFrame* frame, std::string prep_dir, bool add_layout_view)
{
    if(frame == get_widget())
    {
        return ;
    }

    detach();

    if(valid())
    {
        frame->add_layout_view(this, add_layout_view);
        m_enable_attach = add_layout_view ? false : true;
    }
    else
    {
        frame->load_layout_view(this, prep_dir, add_layout_view);
    }

}

void LayoutView::detach()
{
    RenderFrame* frame = this->get_widget();
    if(frame)
    {
        frame->detach_layout_view(this);
        m_enable_attach = true;

    }
}

}
