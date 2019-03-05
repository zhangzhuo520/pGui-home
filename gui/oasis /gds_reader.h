#ifndef GDS_READER_H
#define GDS_READER_H
#include "oasis_types.h"
#include "oasis_util.h"

namespace oasis
{

class GDSReader
{
public:
    GDSReader();
    ~GDSReader();

    void set_fp(FILE* fp) { m_fp = fp; }
    void set_file_offset(int64 offset);
    void reset_buf_size(uint32 size);

    uint8 read_byte();
    uint16 read_uint16();
    void read_bytes(uint8* buf, int32 len);

    int64 get_data_offset() const;

private:
    void setBuffer(ReadBuffer& buffer)
    {
        m_dataCurrent = buffer.current;
        m_dataEnd = buffer.end;
    }
    bool bufferEmpty() const
    {
        return m_dataCurrent == m_dataEnd;
    }
    int32 availData() const
    {
        return m_dataEnd - m_dataCurrent;
    }
    void feedBuffer();

private:
    FILE* m_fp;
    uint8* m_dataCurrent;
    uint8* m_dataEnd;
    ReadBuffer m_fileBuf;
    int64 m_fileOffset;
    uint32 m_buf_size;
};

}
#endif
