#include "ui_navigator.h"
#include "../renderer/render_frame.h"

#include <QHBoxLayout>
#include <QDebug>
namespace ui {

Navigator::Navigator(MainWindow *mainwindow, render::RenderFrame *src_render_frame):
    m_mainwindow(mainwindow),
    m_src_render_frame(src_render_frame)
{
//    resize(100,100);
//    m_render_frame = new render::RenderFrame(this);

//    QHBoxLayout *hlayout = new QHBoxLayout;
//    hlayout->setMargin(0);
//    hlayout->setSpacing(0);
//    hlayout->addWidget(m_render_frame);
//    setLayout(hlayout);
}

Navigator::~Navigator()
{
}

void Navigator::update_source_frame(render::RenderFrame * render_frame)
{
//    if (m_src_render_frame == render_frame)
//    {
//        return;
//    }

//    if (m_src_render_frame != NULL)
//    {
//        delete m_src_render_frame;
//        m_src_render_frame = 0;
//    }
//    m_src_render_frame = render_frame;
//    update_frame_data();
}

void Navigator::update_frame_data()
{
//    if (m_src_render_frame == NULL)
//    {
//        return;
//    }
//   std::vector<render::LayoutView*> layoutview_vector = m_src_render_frame->get_layout_views_list();
//   for (unsigned int i = 0; i < layoutview_vector.size(); i ++)
//   {
//       m_render_frame->set_layout_view(layoutview_vector.at(i), i);
//   }
//   m_render_frame->update();
}

}
