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
    void initialize();
    void prepare_compress();
    void feed_all_data(const uint8* buf, uint32 buflen);
    void fetch_data(uint8* buf, uint32 buflen, uint32& complen);

private:
    z_stream_s* m_zstream;
    bool m_fetch_done;
};

}

#endif
