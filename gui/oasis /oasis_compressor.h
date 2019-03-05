#ifndef OASIS_COMPRESSOR_H
#define OASIS_COMPRESSOR_H
#include "oasis_types.h"
#include <string>

struct z_stream_s;

namespace oasis
{

class OasisCompressor
{
public:
    OasisCompressor();
    ~OasisCompressor();
    int32 initialize();
    int32 prepare_compress();
    void feed_all_data(const uint8* buf, uint32 buflen);
    int32 fetch_data(uint8* buf, uint32 buflen, uint32& complen);

private:
    z_stream_s* m_zstream;
    bool m_fetch_done;
};

}

#endif
