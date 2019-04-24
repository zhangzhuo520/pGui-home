#include "gds_parser.h"
#include "oasis_exception.h"

#define FILE_BUF_SIZE (1024 * 1024)
using namespace oasis;

GDSReader::GDSReader() : m_fileBuf(FILE_BUF_SIZE)
{
    m_dataCurrent = m_dataEnd = NULL;
    m_fileOffset = 0;
    m_buf_size = FILE_BUF_SIZE;
}

GDSReader::~GDSReader()
{

}

uint8 GDSReader::read_byte()
{
    if(bufferEmpty())
    {
        feedBuffer();
    }
    return *m_dataCurrent++;
}

uint16 GDSReader::read_uint16()
{
    uint16 h = read_byte();
    uint16 l = read_byte();
    return ((h << 8) | l);
}

void GDSReader::read_bytes(uint8* buf, int32 len)
{
    while(len > 0)
    {
        if(bufferEmpty())
        {
            feedBuffer();
        }
        int  n = std::min(len, availData());
        memcpy(buf, m_dataCurrent, n);
        buf = buf + n;
        m_dataCurrent += n;
        len -= n;
    }
}

void GDSReader::set_file_offset(int64 offset)
{
    if (fseek(m_fp, offset, SEEK_SET) != 0)
    {
        OAS_DBG << "seek file failed" << OAS_ENDL;
        throw OasisException("seek file failed");
    }
    m_fileOffset = offset;
    m_fileBuf.set_length(0);
    setBuffer(m_fileBuf);
}

void GDSReader::reset_buf_size(uint32 size)
{
    uint32 round_size = 128;
    while (round_size < size && round_size < FILE_BUF_SIZE)
    {
        round_size <<= 1;
    }
    m_buf_size = round_size;
}

int64 GDSReader::get_data_offset() const
{
    return m_fileOffset + (m_dataCurrent - m_fileBuf.begin);
}

void GDSReader::feedBuffer()
{
    oas_assert(bufferEmpty());

    m_fileOffset += m_dataCurrent - m_fileBuf.begin;
    int32 n = fread(m_fileBuf.begin, 1, m_buf_size, m_fp);

    if (n == 0)
    {
        if(feof(m_fp))
        {
        }
        else
        {
            throw OasisException("read file failed");
        }
    }

    m_fileBuf.set_length(n);
    setBuffer(m_fileBuf);

    oas_assert(!bufferEmpty());
}

