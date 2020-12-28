#include "RamSize.hh"

#include <sstream>

namespace gb4e
{
RamSize const RAM_SIZE_INVALID(0xFF, 0, 0);
RamSize const RAM_SIZE_00(0x00, 0, 0);
RamSize const RAM_SIZE_01(0x01, 2 * 1024, 1);
RamSize const RAM_SIZE_02(0x02, 8 * 1024, 1);
RamSize const RAM_SIZE_03(0x03, 32 * 1024, 4);
RamSize const RAM_SIZE_04(0x04, 128 * 1024, 16);
RamSize const RAM_SIZE_05(0x05, 64 * 1024, 8);

RamSize const * ToRamSize(u8 ramSizeByte)
{
    switch (ramSizeByte) {
    case 0x00:
        return &RAM_SIZE_00;
    case 0x01:
        return &RAM_SIZE_01;
    case 0x02:
        return &RAM_SIZE_02;
    case 0x03:
        return &RAM_SIZE_03;
    case 0x04:
        return &RAM_SIZE_04;
    case 0x05:
        return &RAM_SIZE_05;
    }
    return &RAM_SIZE_INVALID;
}

std::string RamSize::ToString() const
{
    std::stringstream ss;
    ss << '{';
    ss << '\t' << "\"byteValue\":" << std::hex << (int)byteValue << ',';
    ss << '\t' << "\"byteSize\":" << std::dec << (int)byteSize << ',';
    ss << '\t' << "\"numBanks\":" << std::dec << (int)numBanks;
    ss << '}';
    return ss.str();
}
};
