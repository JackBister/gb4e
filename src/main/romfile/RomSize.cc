#include "RomSize.hh"

#include <sstream>

namespace gb4e
{
RomSize const ROM_SIZE_INVALID(0xFF, 0, 0);
RomSize const ROM_SIZE_00(0x00, 32 * 1024, 2);
RomSize const ROM_SIZE_01(0x01, 64 * 1024, 4);
RomSize const ROM_SIZE_02(0x02, 128 * 1024, 8);
RomSize const ROM_SIZE_03(0x03, 256 * 1024, 16);
RomSize const ROM_SIZE_04(0x04, 512 * 1024, 32);
RomSize const ROM_SIZE_05(0x05, 1024 * 1024, 64);
RomSize const ROM_SIZE_06(0x06, 2048 * 1024, 128);
RomSize const ROM_SIZE_07(0x07, 4096 * 1024, 256);
RomSize const ROM_SIZE_08(0x08, 8192 * 1024, 512);
RomSize const ROM_SIZE_52(0x52, 1152 * 1024, 72);
RomSize const ROM_SIZE_53(0x53, 1280 * 1024, 80);
RomSize const ROM_SIZE_54(0x54, 1536 * 1024, 96);

RomSize const * ToRomSize(u8 romSizeByte)
{
    switch (romSizeByte) {
    case 0x00:
        return &ROM_SIZE_00;
    case 0x01:
        return &ROM_SIZE_01;
    case 0x02:
        return &ROM_SIZE_02;
    case 0x03:
        return &ROM_SIZE_03;
    case 0x04:
        return &ROM_SIZE_04;
    case 0x05:
        return &ROM_SIZE_05;
    case 0x06:
        return &ROM_SIZE_06;
    case 0x07:
        return &ROM_SIZE_07;
    case 0x08:
        return &ROM_SIZE_08;
    case 0x52:
        return &ROM_SIZE_52;
    case 0x53:
        return &ROM_SIZE_53;
    case 0x54:
        return &ROM_SIZE_54;
    }
    return &ROM_SIZE_INVALID;
}

std::string RomSize::ToString() const
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
