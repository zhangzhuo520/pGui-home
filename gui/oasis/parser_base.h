#ifndef PARSER_BASE_H
#define PARSER_BASE_H
#include "oasis_types.h"
#include "oasis_parse_option.h"
#include "thread_base.h"

namespace oasis
{
class OasisLayout;
class ParserBase
{
public:
    void set_parse_option(OasisParseOption option) { m_parse_option = option; }
    void set_layout(OasisLayout* layout) { m_layout = layout; }

    void parse_all_mthread();
    void parse_worker();
    void dispatcher();
    virtual void parse_cell_concurrent(int64 info_index) = 0;

protected:
    OasisParseOption m_parse_option;
    std::string m_filename;
    OasisLayout* m_layout;

private:
    sys::Mutex m_parse_locker;
    sys::Condition m_parse_condition;
    sys::Mutex m_done_locker;
    bool m_dispatch_done;
    std::list<int64> m_parse_queue;
    std::set<int64> m_done_set;
};
}

#endif
