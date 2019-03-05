#include "oasis_compressor.h"
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

int32 OasisCompressor::initialize()
{
    int rc = deflateInit2(m_zstream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
    if(rc != Z_OK)
    {
        delete m_zstream;
        m_zstream = NULL;
        OAS_DBG << "initialize compressor failed" << OAS_ENDL;
        return -1;
    }

    return 0;
}

int32 OasisCompressor::prepare_compress()
{
    m_zstream->next_in = NULL;
    m_zstream->avail_in = 0;
    int rc = deflateReset(m_zstream);
    if(rc != Z_OK)
    {
        OAS_DBG << "prepareCompress failed" << OAS_ENDL;
        return -1;
    }

    return 0;
}

void OasisCompressor::feed_all_data(const uint8* buf, uint32 buflen)
{
    oas_assert(buf != NULL && buflen > 0);
    oas_assert(m_zstream->avail_in == 0);

    m_zstream->next_in = const_cast<Bytef*>(buf);
    m_zstream->avail_in = buflen;

    m_fetch_done = false;
}

int32 OasisCompressor::fetch_data (uint8* buf, uint32 buflen, uint32& complen)
{
    complen = 0;
    oas_assert(buf != NULL && buflen > 0);

    if(m_fetch_done)
    {
        return 0;
    }

    m_zstream->next_out = reinterpret_cast<Bytef*>(buf);
    m_zstream->avail_out = buflen;
    int rc = deflate(m_zstream, Z_FINISH);
    if(rc == Z_STREAM_END)
    {
        m_fetch_done = true;
    }
    else if(rc != Z_OK)
    {
        OAS_DBG << "fetch data failed " << rc << OAS_ENDL;
        return -1;
    }

    complen = m_zstream->next_out - reinterpret_cast<Bytef*>(buf);
    oas_assert(complen != 0 || m_zstream->avail_in == 0);
    return 0;
}


