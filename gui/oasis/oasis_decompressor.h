#ifndef OASIS_DECOMPRESSOR_H
#define OASIS_DECOMPRESSOR_H
#include "oasis_types.h"
#include <string>

struct z_stream_s;

namespace oasis
{

class OasisDecompressor
{
public:
    OasisDecompressor();
    ~OasisDecompressor();
    void initialize();
    void prepare_decompress();
    void feed_all_data(uint8* buf, uint32 buflen);
    void fetch_data(uint8* buf, uint32 buflen, uint32& uncomplen);

private:
    z_stream_s* m_zstream;
    bool m_fetch_done;
};

}

#endif
