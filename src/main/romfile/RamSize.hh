#pragma once

#include <string>

#include "Common.hh"

namespace gb4e
{
class RamSize
{
public:
    RamSize(u8 byteValue, size_t byteSize, u8 numBanks) : byteValue(byteValue), byteSize(byteSize), numBanks(numBanks)
    {
    }

    u8 GetByteValue() const { return byteValue; }
    size_t GetByteSize() const { return byteSize; }
    u8 GetNumBanks() const { return numBanks; }
    bool IsValid() const { return byteValue != 0xFF; }
    std::string ToString() const;

private:
    u8 byteValue;
    size_t byteSize;
    u8 numBanks;
};
RamSize const * ToRamSize(u8 ramSizeByte);
};
