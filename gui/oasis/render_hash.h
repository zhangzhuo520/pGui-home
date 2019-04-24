#ifndef RENDER_HASH_H
#define RENDER_HASH_H

#include  "oasis_types.h"
#include <boost/unordered_map.hpp>
namespace render
{
    struct hash_LDType
    {
        size_t operator()(const oasis::LDType& ld) const
        {
            std::size_t seed = 0;
            boost::hash_combine(seed, boost::hash_value(ld.layer));
            boost::hash_combine(seed, boost::hash_value(ld.datatype));
            return seed;
        }
    };

    struct cmp_LDType
    {
        bool operator()(const oasis::LDType& ld1, const oasis::LDType& ld2) const
        {
            return ld1.datatype == ld2.datatype && ld1.layer == ld2.layer;
        }
    };
}
#endif // RENDER_HASH_H
