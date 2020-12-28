#pragma once

#include <string>

#include "Common.hh"

namespace gb4e
{
u8 const SGB_FLAG_NO_SGB_SUPPORT_VALUE = 0x0;
u8 const SGB_FLAG_SGB_SUPPORT_VALUE = 0x3;

enum class SgbFlag { SGB_SUPPORT, NO_SGB_SUPPORT, UNKNOWN };

SgbFlag ToSgbFlag(u8 sgbFlagByte);
std::string ToString(SgbFlag sgbFlag);
};