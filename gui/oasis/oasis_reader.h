#ifndef OASIS_READER_H
#define OASIS_READER_H
#include "oasis_types.h"
#include "oasis_decompressor.h"
#include "oasis_util.h"
#include <string.h>

namespace oasis
{

class OasisReader
{
public:
    OasisReader();
    ~OasisReader();

    void init(FILE* fp, int64 offset = 0);
    void reset_buf_size(uint64 size);

    uint8 read_byte();
    uint8 peek_byte();
    void read_bytes(uint8* buf, int32 len);
    void skip_bytes(int32 len);

    uint64 read_uint();
    int64 read_int();

    void skip_string();
    void read_string(OasisString& ostring);

    void read_real(OasisReal& real);

    void read_delta_1(OasisDelta& delta, bool horizontal);

    void read_delta_2(OasisDelta& delta);

    void read_delta_3(OasisDelta& delta);

    void read_delta_g(OasisDelta& delta);

    void read_repetition(OasisRepetition& repetition);

    void read_point_list(OasisPointList& pointlist);

    void read_prop_value(OasisPropValue& propvalue);

    void read_interval(OasisInterval& interval);

    void enter_cblock(uint64 uncomp_byte_count, uint64 comp_byte_count);
    bool in_cblock() const { return m_in_cblock; }
    int64 get_data_offset();
    int64 get_file_size();
    void seek(int64 offset, bool enforce = false);

private:
    void set_buffer(ReadBuffer& buffer)
    {
        m_data_current = buffer.current;
        m_data_end = buffer.end;
    }
    void get_buffer(ReadBuffer& buffer) const
    {
        buffer.current = m_data_current;
        buffer.end = m_data_end;
    }
    bool buffer_empty() const
    {
        return m_data_current == m_data_end;
    }
    int32 available_data() const
    {
        return m_data_end - m_data_current;
    }
    void feed_buffer();
    void feed_more_file_buffer(int size);
    void feed_file_buffer();
    void feed_cblock_buffer();

    void leave_cblock() { m_in_cblock = false; }

private:
    class Buffer
    {
    public:
        Buffer(uint32 len = 0) : m_data(NULL), m_len(len)
        {
            if(len > 0) m_data = new uint8[len];
        }
        ~Buffer()
        {
            delete [] m_data;
        }
        uint8* Get()
        {
            return m_data;
        }
        void Reserve(uint32 len)
        {
            if(len > m_len)
            {
                delete [] m_data;
                m_data = new uint8[len];
                m_len = len;
            }
        }
    private:
        Buffer(const Buffer&);
        Buffer& operator=(const Buffer&);
        uint8* m_data;
        uint32 m_len;
    };

    Buffer m_byte_buffer;
    ReadBuffer m_file_buffer;
    ReadBuffer m_cblock_buffer;

    uint8* m_data_current;
    uint8* m_data_end;

    OasisDecompressor* m_decompressor;
    bool m_in_cblock;
    uint64 m_uncompressed_bytes_left;
    uint64 m_compressed_bytes_left;

    FILE* m_fp;
    int64 m_file_offset;

    uint64 m_file_buffer_size;
};

}

#endif
