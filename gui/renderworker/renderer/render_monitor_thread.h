#ifndef RENDER_MONITOR_THREAD_H
#define RENDER_MONITOR_THREAD_H

#include <QThread>
#include <vector>

namespace render
{

class LayoutView;

class MonitorThread: public QThread
{
public:
    MonitorThread(render::LayoutView* lv);
    void closeThread();
protected:
    virtual void run();

private:
    bool m_isStop;
    render::LayoutView* m_lv;
};


}




#endif // RENDER_MONITOR_THREAD_H
