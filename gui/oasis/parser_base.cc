#include "parser_base.h"
#include "oasis_layout.h"
#include "thread_worker.h"
#include <unistd.h>

using namespace oasis;

void ParserBase::parse_all_mthread()
{
    m_dispatch_done = false;
    sys::Thread* dispatcher = new sys::WorkThread<ParserBase, void>(*this, &ParserBase::dispatcher);
    dispatcher->SetName("DispatchCell");
    dispatcher->Start();
    std::vector<sys::Thread*> workers;
    for (uint32 i = 0; i < m_parse_option.thread_count(); ++i)
    {
        sys::Thread* worker = new sys::WorkThread<ParserBase, void>(*this, &ParserBase::parse_worker);
        worker->SetName("ParseCell");
        worker->Start();
        workers.push_back(worker);
    }
    for (uint32 i = 0; i < workers.size(); ++i)
    {
        sys::Thread* worker = workers[i];
        worker->Wait();
        delete worker;
    }
    workers.clear();
    dispatcher->Wait();
    delete dispatcher;
}

void ParserBase::dispatcher()
{
    OAS_INFO << "begin dispatcher " << OAS_ENDL;
    uint32 info_size = m_layout->m_cellinfos.size();
    std::set<int64> pending_set;
    for (uint32 i = 0; i < info_size; ++i)
    {
        int64 info_index = i;
        if (m_layout->m_info2children.count(info_index) == 0 || m_layout->m_info2children[info_index].empty())
        {
            m_parse_locker.Lock();
            m_parse_queue.push_back(info_index);
            m_parse_condition.Signal();
            m_parse_locker.Unlock();
        }
        else
        {
            pending_set.insert(info_index);
        }
    }
    OAS_INFO << "total cell " << info_size << " pending size " << pending_set.size() << OAS_ENDL;
    while (!pending_set.empty())
    {
        m_done_locker.Lock();
        std::set<int64> done_set;
        done_set.swap(m_done_set);
        m_done_locker.Unlock();
        if (done_set.empty())
        {
            usleep(10);
            continue;
        }
        for (auto it = done_set.begin(); it != done_set.end(); ++it)
        {
            int64 cindex = *it;
            std::set<int64> parents = m_layout->m_info2parents[cindex];
            for (auto pit = parents.begin(); pit != parents.end(); ++pit)
            {
                int64 pindex = *pit;
                m_layout->m_info2children[pindex].erase(cindex);
                if (m_layout->m_info2children[pindex].empty() && pending_set.count(pindex))
                {
                    pending_set.erase(pindex);
                    m_parse_locker.Lock();
                    m_parse_queue.push_back(pindex);
                    m_parse_condition.Signal();
                    m_parse_locker.Unlock();
                }
            }
        }
    }
    m_dispatch_done = true;
    m_parse_condition.Broadcast();
}

void ParserBase::parse_worker()
{
    int count = 0;
    while (true)
    {
        m_parse_locker.Lock();
        if (m_parse_queue.empty() && m_dispatch_done)
        {
            m_parse_locker.Unlock();
            break;
        }

        if (m_parse_queue.empty())
        {
            m_parse_condition.Wait(m_parse_locker);
        }

        if (m_parse_queue.empty() && m_dispatch_done)
        {
            m_parse_locker.Unlock();
            break;
        }
        else if(m_parse_queue.empty())
        {
            m_parse_locker.Unlock();
            continue;
        }

        int64 info_index = m_parse_queue.front();
        m_parse_queue.pop_front();
        m_parse_locker.Unlock();

        parse_cell_concurrent(info_index);

        m_done_locker.Lock();
        m_done_set.insert(info_index);
        m_done_locker.Unlock();
        OAS_DBG << "thread " << long(sys::Thread::Self()) << " parse " << ++count << " cells" << OAS_ENDL;
    }
}

