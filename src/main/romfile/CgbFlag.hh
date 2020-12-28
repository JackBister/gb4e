#pragma once

#include <string>

#include "Common.hh"

namespace gb4e
{
u8 const CGB_FLAG_CGB_ONLY_VALUE = 0xC0;
u8 const CGB_FLAG_EITHER_OR_VALUE = 0x80;

enum class CgbFlag { CGB_ONLY, EITHER_OR, UNKNOWN };

CgbFlag ToCgbFlag(u8 cgbFlagByte);
std::string ToString(CgbFlag cgbFlag);
};
