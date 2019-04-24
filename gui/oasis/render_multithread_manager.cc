#include "render_multithread_manager.h"
#include <limits>
#include <QDebug>
#include <memory>

namespace render
{

class CloseTask: public Task
{
public:
    CloseTask() { }
};

class StartTask: public Task
{
public:
    StartTask() { }
};


struct ThreadStoppedException { };
struct TaskStoppedException { };

RenderThread::RenderThread():m_manager(0), m_render_index(-1), m_idle(true),m_stop_apply(false)
{

}

RenderThread::~RenderThread()
{

}

void RenderThread::start(ThreadManager* m, int render_index)
{
    m_manager = m;
    m_render_index = render_index;
    QThread::start();
}

//sentinel is used to check the status whether thread is stopped.
void RenderThread::sentinel()
{
    if(m_stop_apply)
    {
//        qDebug() << "stop request";
        throw TaskStoppedException();
    }
}

void RenderThread::run()
{
    while(true)
    {
        try
        {
            std::auto_ptr<Task> task(m_manager->get_task(m_render_index));
            do_task(task.get());
        }
        catch(TaskStoppedException)
        {

        }
        catch(ThreadStoppedException)
        {
//            qDebug() << "stop request";
            break;
        }
        catch(std::exception &exc)
        {
            qDebug() << exc.what();
        }
        catch(...)
        {
            qDebug() << "unexpected error";
        }
    }
}

void RenderThread::reset_stop_apply()
{
    m_stop_apply = false;
}

void RenderThread::apply_stop()
{
    m_stop_apply = true;
}

ThreadManager::ThreadManager(int thread_nums): m_thread_nums(thread_nums), m_idle_threads(0),m_stop(false), m_working(false)
{
    if(m_thread_nums > 0)
    {
        m_per_thread_task_queue = new TaskQueue[m_thread_nums];
    }
    else
    {
        m_per_thread_task_queue = 0;
    }
}

ThreadManager::~ThreadManager()
{
    interrupt();

    if(m_per_thread_task_queue)
    {
        delete[] m_per_thread_task_queue;
        m_per_thread_task_queue = 0;
    }
}

void ThreadManager::set_threads(int nums)
{
    interrupt();

    m_thread_nums = nums;
    m_idle_threads = 0;

    if(m_per_thread_task_queue)
    {
        delete[] m_per_thread_task_queue;
        m_per_thread_task_queue = 0;
    }

    if(nums > 0)
    {
        m_per_thread_task_queue = new TaskQueue[m_thread_nums];
    }
    else
    {
        m_per_thread_task_queue = 0;
    }
}

bool ThreadManager::busy() const
{
    return m_working;
}

bool ThreadManager::wait(long time)
{
    bool result = true;

    m_global_lock.lock();
    if(m_thread_nums > 0 && m_working && !m_queue_empty_condition.wait(&m_global_lock, time >= 0? (unsigned long) time : std::numeric_limits<unsigned long>::max()))
    {
        result = false;
    }
    m_global_lock.unlock();
    return result;
}

Task* ThreadManager::get_task(int index)
{
    while(true)
    {
        m_global_lock.lock();
        while(m_task_queue.empty() && m_per_thread_task_queue[index].empty())
        {
            m_idle_threads++;
            if(m_idle_threads == m_thread_nums)
            {
                // when manager is idle and not stoppped.
                // we can close the manager.
                if(!m_stop)
                {
                    qDebug() << "get_task call finished.";
                    finished();
                }

                m_working = false;
                qDebug() << "get_task() queue empty condition wake all";
                m_queue_empty_condition.wakeAll();
            }

            while(m_task_queue.empty() && m_per_thread_task_queue[index].empty())
            {
                // receive a task will wake up the condition variable.
                m_threads[index]->set_idle(true);
                m_task_acessible_condition.wait(&m_global_lock);
                m_threads[index]->set_idle(false);
            }
            m_idle_threads--;
        }

//        qDebug() << "renderer:" << index << m_threads[index]->idle() << "get_task task num:" << m_task_queue.tasks();
        Task* task = !m_per_thread_task_queue[index].empty() ? m_per_thread_task_queue[index].get_next_task() : (! m_task_queue.empty() ? m_task_queue.get_next_task() : 0);

        m_global_lock.unlock();

        if(dynamic_cast<CloseTask*> (task) != 0)
        {
            delete task;
            throw ThreadStoppedException();
        }
        else if(dynamic_cast<StartTask*> (task) != 0)
        {
            delete task;
        }
        else if(task)
        {
            return task;
        }
    }
}

void ThreadManager::push(Task *task)
{
    m_global_lock.lock();

    //ThreadManager is not allowed to work on stop status;
    if(m_stop)
    {
        delete task;
    }
    else
    {
        m_task_queue.push(task);

        if(m_working)
        {
            m_task_acessible_condition.wakeAll();
        }
    }
//    qDebug() << "task num:" << m_task_queue.tasks();
    m_global_lock.unlock();
}

void ThreadManager::stop()
{
    qDebug() << "ThreadManager::stop() ready, current tasks are:" << m_task_queue.tasks()
             << "whether is working:" << m_working;

    if(!m_working)
    {
        return;
    }

    m_global_lock.lock();

    m_stop = true;

    //remove all not done tasks

    while(!m_task_queue.empty())
    {
        Task* task = m_task_queue.get_next_task();
        delete task;
    }

    if(m_threads.size() != 0)
    {
        bool result = false;
        for(size_t i = 0 ; i < m_threads.size(); i++)
        {
            if(!m_threads[i]->idle())
            {
                m_threads[i]->apply_stop();
                result = true;
            }
        }

        if(result)
        {
            // sentinel will create closeTask, hence we need to wait.
            m_task_acessible_condition.wakeAll();

            //wait for the idle thread to handle the closeTask.
            m_queue_empty_condition.wait(&m_global_lock);
        }
    }

    qDebug() << "ThreadManager::stop() finish, current task nums:" << m_task_queue.tasks();

    //recover to the initialization status
    m_stop = false;
    m_working = false;

    m_global_lock.unlock();

    closed();
}

void ThreadManager::interrupt()
{
    stop();

    if(!m_threads.empty())
    {
        m_global_lock.lock();

        for(size_t i = 0; i < m_threads.size(); i++)
        {
            m_threads[i]->apply_stop();
            m_per_thread_task_queue[i].push(new CloseTask());
        }

        m_task_acessible_condition.wakeAll();
        m_global_lock.unlock();

        for(size_t i = 0; i < m_threads.size(); i++)
        {
            //QThread::wait()
            m_threads[i]->wait();
        }

        for(std::vector<RenderThread*>::iterator it = m_threads.begin(); it != m_threads.end(); it++)
        {
            delete (*it);
            *it = 0;
        }

        m_threads.clear();
        std::vector<RenderThread*>().swap(m_threads);
    }
}

void ThreadManager::start()
{
    m_global_lock.lock();

    m_working = true;

    for(int i = 0; i < m_thread_nums;i++)
    {
        m_per_thread_task_queue[i].push_front(new StartTask());
    }

    m_task_acessible_condition.wakeAll();

    while(m_thread_nums > int(m_threads.size()))
    {
        m_threads.push_back(create_thread());
        setup_thread(m_threads.back());
        m_threads.back()->reset_stop_apply();
        m_threads.back()->start(this, int(m_threads.size() - 1));
    }

    for(size_t i = 0; i < m_threads.size(); i++)
    {
        setup_thread(m_threads[i]);
        m_threads[i]->reset_stop_apply();
    }

    m_global_lock.unlock();

    if(m_threads.empty())
    {
        std::auto_ptr<RenderThread> thread(create_thread());
        setup_thread(thread.get());

        while(!m_task_queue.empty())
        {
            try
            {
                std::auto_ptr<Task> task(m_task_queue.get_next_task());
                thread->do_task(task.get());
            }
            catch(TaskStoppedException)
            {
                break;
            }
            catch (ThreadStoppedException)
            {
                break;
            }
            catch(std::exception &exc)
            {
                qDebug() << exc.what();
            }
            catch(...)
            {
                qDebug() << "unexpected error appears.";
            }
        }

        while(!m_task_queue.empty())
        {
            Task* task = m_task_queue.get_next_task();
            if(task)
            {
                delete task;
                task = 0;
            }
        }

//        qDebug() << "Thread Manager start finished.";
        finished();
        m_working = false;
    }
}

}
