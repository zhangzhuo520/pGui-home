#ifndef OASIS_RECORD_H
#define OASIS_RECORD_H
#include "oasis_types.h"
#include <string>

namespace oasis
{
std::string oasis_record_id_name(const OasisRecordId& rid);
std::string gds_record_id_name(const GDSRecordId& rid);
}

#endif
