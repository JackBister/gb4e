#include "Common.hh"

#include <sstream>

namespace gb4e
{
std::string FlagsToString(u8 flags)
{
    if (flags == 0) {
        return "0";
    }
    std::stringstream ss;
    ss << ((flags & FLAG_ZERO) ? 'Z' : '-');
    ss << ((flags & FLAG_N) ? 'N' : '-');
    ss << ((flags & FLAG_HC) ? 'H' : '-');
    ss << ((flags & FLAG_C) ? 'C' : '-');
    return ss.str();
}
}
