#ifndef RENDER_TASK_H
#define RENDER_TASK_H

#include <deque>

namespace render
{

class Task;

class TaskQueue
{
public:
    TaskQueue();

    ~TaskQueue();

    bool empty() const
    {
        return m_start_index == m_end_index;
    }

    int tasks() const
    {
        return m_end_index - m_start_index;
    }
    Task* get_next_task();

    void push(Task* task);

    void push_front(Task* task);

    const Task* get_head_task()
    {
        return m_task_queue[m_start_index];
    }

private:
    int m_start_index;
    int m_end_index;
    std::deque<Task*> m_task_queue;

    TaskQueue(const TaskQueue&);
    TaskQueue &operator=(const TaskQueue& );
    void cleanup();
};

class Task
{
public:
    Task() {}

    virtual ~Task() {}
};

}
#endif // RENDER_TASK_H
