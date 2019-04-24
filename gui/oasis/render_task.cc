#include "render_task.h"

namespace render
{

TaskQueue::TaskQueue()
{
    m_start_index = -1;
    m_end_index = -1;
}

TaskQueue::~TaskQueue()
{
    while(!empty())
    {
        Task* task = get_next_task();
        delete task;
    }
}

Task* TaskQueue::get_next_task()
{
    if(m_start_index == m_end_index)
    {
        return (Task*) 0;
    }
    Task* result = m_task_queue[m_start_index];
    m_start_index ++;
    return result;
}

void TaskQueue::push(Task* task)
{
    m_task_queue.push_back(task);
    if(m_start_index == -1)
    {
        m_start_index = 0;
        m_end_index = 1;
    }
    else
    {
        m_end_index++;
    }
}

void TaskQueue::push_front(Task* task)
{
    if(m_start_index == -1)
    {
        m_start_index = 0;
        m_end_index = 1;
    }
    else
    {
        m_end_index++;
    }

    auto it = m_task_queue.begin();
    m_task_queue.insert(it + m_start_index, task);

}

}
