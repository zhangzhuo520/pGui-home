#ifndef RENDER_MULTITHREAD_MANAGER_H
#define RENDER_MULTITHREAD_MANAGER_H

#include "render_task.h"
#include <QMutex>
#include <QWaitCondition>
#include <QThread>


namespace render
{

class ThreadManager;

class RenderThread: protected QThread
{
public:
    RenderThread();

    virtual ~RenderThread();

    int render_index() const
    {
        return m_render_index;
    }

    void sentinel();

protected:
    virtual void do_task(Task* task) = 0;

    bool stop_applied() const
    {
        return m_stop_apply;
    }

    bool idle() const
    {
        return m_idle;
    }

    void set_idle(bool status)
    {
        m_idle = status;
    }


private:
    friend class ThreadManager;
    virtual void run();
    void apply_stop();
    void reset_stop_apply();
    void start(ThreadManager* m, int render_index);

    ThreadManager* m_manager;
    int m_render_index;
    bool m_idle;
    bool m_stop_apply;
};

class ThreadManager
{
public:
    ThreadManager(int thread_nums);

    virtual ~ThreadManager();

    int thread_nums()
    {
        return m_thread_nums;
    }

    void set_threads(int thread_nums);

    bool busy() const;

    void stop();

    void push(Task* task);

    void start();

    bool wait(long time = -1);

    void interrupt();

    int tasks() const {return m_task_queue.tasks();}

protected:
    virtual RenderThread* create_thread() = 0;

    virtual void setup_thread(RenderThread* thread) { }

    virtual void finished() { }

    virtual void closed() { }

    RenderThread* get_thread(int index) const
    {
        return m_threads[index];
    }


private:
    friend class RenderThread;

    TaskQueue m_task_queue;
    TaskQueue *m_per_thread_task_queue;

    int m_thread_nums;
    int m_idle_threads;
    bool m_stop;
    bool m_working;

    QMutex m_global_lock;
    QWaitCondition m_task_acessible_condition;
    QWaitCondition m_queue_empty_condition;

    std::vector<RenderThread* > m_threads;

    Task* get_task(int index);
};

}
#endif // RENDER_MULTITHREAD_MANAGER_H
