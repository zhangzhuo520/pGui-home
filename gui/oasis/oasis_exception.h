#ifndef OASIS_EXCEPTION_H
#define OASIS_EXCEPTION_H
#include <stdexcept>

namespace oasis
{

class OasisException : public std::runtime_error
{
public:
    OasisException(const std::string& what_arg) : std::runtime_error(what_arg) {}
};

}

#endif
