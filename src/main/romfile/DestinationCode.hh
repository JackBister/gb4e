#pragma once

#include <string>

#include "Common.hh"

namespace gb4e
{
enum class DestinationCode { JAPAN, NOT_JAPAN, UNKNOWN };

DestinationCode ToDestinationCode(u8 destinationCodeByte);
std::string ToString(DestinationCode destinationCode);
};
