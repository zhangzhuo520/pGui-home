#include "oasis_compressor.h"
#include "oasis_exception.h"
#include <zlib.h>

using namespace std;
using namespace oasis;

OasisCompressor::OasisCompressor()
{
    m_zstream = new z_stream;
    m_zstream->zalloc = NULL;
    m_zstream->zfree = NULL;
    m_zstream->next_in = NULL;
    m_zstream->avail_in = 0;
}

OasisCompressor::~OasisCompressor()
{
    deflateEnd(m_zstream);
    delete m_zstream;
}

void OasisCompressor::initialize()
{
    int rc = deflateInit2(m_zstream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
    if(rc != Z_OK)
    {
        throw OasisException("compressor initialize failed");
    }
}

void OasisCompressor::prepare_compress()
{
    m_zstream->next_in = NULL;
    m_zstream->avail_in = 0;
    int rc = deflateReset(m_zstream);
    if(rc != Z_OK)
    {
        throw OasisException("prepare compress failed");
    }
}

void OasisCompressor::feed_all_data(const uint8* buf, uint32 buflen)
{
    oas_assert(buf != NULL && buflen > 0);
    oas_assert(m_zstream->avail_in == 0);

    m_zstream->next_in = const_cast<Bytef*>(buf);
    m_zstream->avail_in = buflen;

    m_fetch_done = false;
}

void OasisCompressor::fetch_data(uint8* buf, uint32 buflen, uint32& complen)
{
    complen = 0;

    if(m_fetch_done)
    {
        return;
    }

    oas_assert(buf != NULL && buflen > 0);

    m_zstream->next_out = reinterpret_cast<Bytef*>(buf);
    m_zstream->avail_out = buflen;
    int rc = deflate(m_zstream, Z_FINISH);
    if(rc == Z_STREAM_END)
    {
        m_fetch_done = true;
    }
    else if(rc != Z_OK)
    {
        throw OasisException("fetch data failed");
    }

    complen = m_zstream->next_out - reinterpret_cast<Bytef*>(buf);
    oas_assert(complen != 0 || m_zstream->avail_in == 0);
}


