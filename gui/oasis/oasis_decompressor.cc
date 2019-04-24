#include "oasis_decompressor.h"
#include "oasis_exception.h"
#include <zlib.h>

using namespace std;
using namespace oasis;

OasisDecompressor::OasisDecompressor()
{
    m_zstream = new z_stream;
    m_zstream->zalloc = NULL;
    m_zstream->zfree = NULL;
    m_zstream->next_in = NULL;
    m_zstream->avail_in = 0;
}

OasisDecompressor::~OasisDecompressor()
{
    inflateEnd(m_zstream);
    delete m_zstream;
}

void OasisDecompressor::initialize()
{
    int rc = inflateInit2(m_zstream, -MAX_WBITS);
    if (rc != Z_OK)
    {
        throw OasisException("initialize decompress failed");
    }
}

void OasisDecompressor::prepare_decompress()
{
    m_zstream->next_in = NULL;
    m_zstream->avail_in = 0;
    int rc = inflateReset(m_zstream);
    if(rc != Z_OK)
    {
        throw OasisException("prepare decompress failed");
    }
}

void OasisDecompressor::feed_all_data(uint8* buf, uint32 buflen)
{
    oas_assert(buf != NULL && buflen > 0);
    oas_assert(m_zstream->avail_in == 0);

    m_zstream->next_in = reinterpret_cast<Bytef*>(buf);
    m_zstream->avail_in = buflen;

    m_fetch_done = false;
}

void OasisDecompressor::fetch_data (uint8* buf, uint32 buflen, uint32& uncomplen)
{
    uncomplen = 0;

    if (m_fetch_done)
    {
        return;
    }

    oas_assert(buf != NULL && buflen > 0);

    m_zstream->next_out = reinterpret_cast<Bytef*>(buf);
    m_zstream->avail_out = buflen;

    int rc = inflate(m_zstream, Z_SYNC_FLUSH);

    if(rc == Z_STREAM_END)
    {
        m_fetch_done = true;
    }
    else if(rc != Z_OK)
    {
        throw OasisException("fetch data failed");
    }

    uncomplen = (m_zstream->next_out - reinterpret_cast<Bytef*>(buf));
    oas_assert(uncomplen > 0 || m_zstream->avail_in == 0);
}

