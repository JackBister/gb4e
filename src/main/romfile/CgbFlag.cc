#include "CgbFlag.hh"

namespace gb4e
{
CgbFlag ToCgbFlag(u8 cgbFlagByte)
{
    if (cgbFlagByte == CGB_FLAG_CGB_ONLY_VALUE) {
        return CgbFlag::CGB_ONLY;
    }
    if (cgbFlagByte == CGB_FLAG_EITHER_OR_VALUE) {
        return CgbFlag::EITHER_OR;
    }
    return CgbFlag::UNKNOWN;
}

std::string ToString(CgbFlag cgbFlag)
{
    switch (cgbFlag) {
    case CgbFlag::CGB_ONLY:
        return "CgbFlag::CGB_ONLY";
    case CgbFlag::EITHER_OR:
        return "CgbFlag::EITHER_OR";
    case CgbFlag::UNKNOWN:
    default:
        return "CgbFlag::UNKNOWN";
    }
}
};
