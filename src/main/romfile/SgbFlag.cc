#include "SgbFlag.hh"

namespace gb4e
{
SgbFlag ToSgbFlag(u8 sgbFlagByte)
{
    if (sgbFlagByte == SGB_FLAG_NO_SGB_SUPPORT_VALUE) {
        return SgbFlag::NO_SGB_SUPPORT;
    }
    if (sgbFlagByte == SGB_FLAG_SGB_SUPPORT_VALUE) {
        return SgbFlag::SGB_SUPPORT;
    }
    return SgbFlag::UNKNOWN;
}

std::string ToString(SgbFlag sgbFlag)
{
    switch (sgbFlag) {
    case SgbFlag::NO_SGB_SUPPORT:
        return "SgbFlag::NO_SGB_SUPPORT";
    case SgbFlag::SGB_SUPPORT:
        return "SgbFlag::SGB_SUPPORT";
    case SgbFlag::UNKNOWN:
    default:
        return "SgbFlag::UNKNOWN";
    }
}
};