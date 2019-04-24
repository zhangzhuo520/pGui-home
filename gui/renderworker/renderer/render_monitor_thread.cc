#include "render_monitor_thread.h"
#include "render_layout_view.h"
#include <QDebug>
namespace render
{

MonitorThread::MonitorThread(render::LayoutView* lv):m_isStop(false), m_lv(lv)
{
}

void MonitorThread::closeThread()
{
    m_isStop = true;
}


void MonitorThread::run()
{
    while(true)
    {
        oasis::OasisLayout* layout = m_lv->get_layout();
        layout->smart_clean_cache(100000, 4, 3);

        if(m_isStop)
        {
            layout->smart_clean_cache(10000, 4, 1000);
//            layout->smart_clean_cache(-1, 0, 1);
            qDebug() << "Monitor Thread close with layout elements:" << layout->imported_element_count();
            return ;
        }

    }
}


}
