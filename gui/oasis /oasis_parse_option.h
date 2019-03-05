#ifndef OASISPARSEOPTION_H
#define OASISPARSEOPTION_H
#include "oasis_types.h"
#include <set>
#include <map>

namespace oasis
{
class OasisParseOption
{
public:
    OasisParseOption() : m_parse_over(false), m_parse_text(false), m_do_partition(true), m_compress_geometry(true), m_thread_count(0) {}

    void parse_over(bool enable) { m_parse_over = enable; }
    bool parse_over() const { return m_parse_over; }

    void parse_text(bool enable) { m_parse_text = enable; }
    bool parse_text() const { return m_parse_text; }

    void do_partition(bool enable) { m_do_partition = enable; }
    bool do_partition() const { return m_do_partition; }

    void compress_geometry(bool enable) { m_compress_geometry = enable; }
    bool compress_geometry() const { return m_compress_geometry; }

    void thread_count(uint32 count) { m_thread_count = count; }
    uint32 thread_count() const { return m_thread_count; }

    void add_layer(int64 layer, int64 datatype) { m_lds.insert(LDType(layer, datatype)); }
    bool include_layer(int64 layer, int64 datatype)
    {
        return m_lds.empty() || m_lds.find(LDType(layer, datatype)) != m_lds.end();
    }

    void map_layer(const LDType& from, const LDType& to) { m_layer_maps[from] = to; }
    LDType map_layer(int64 layer, int64 datatype)
    {
        LDType ld(layer, datatype);
        if(m_layer_maps.find(ld) != m_layer_maps.end())
        {
            return m_layer_maps[ld];
        }
        else
        {
            return ld;
        }
    }
private:
    std::set<LDType> m_lds;
    std::map<LDType, LDType> m_layer_maps;
    bool m_parse_over;
    bool m_parse_text;
    bool m_do_partition;
    bool m_compress_geometry;
    uint32 m_thread_count;
};

}
#endif
