#pragma once

#include <array>
#include <optional>

#include "Common.hh"

namespace gb4e
{
class RomFile;

size_t constexpr CARTRIDGE_RAM_BANK_SIZE = 0x2000;
size_t constexpr CARTRIDGE_ROM_BANK_SIZE = 0x4000;

class Cartridge
{
public:
    void LoadRom(RomFile const *);

    bool WriteMemory(u16 addr, u8 val);
    std::optional<u8> ReadMemory(u16 addr) const;

private:
    RomFile const * romFile;

    std::array<std::array<u8, CARTRIDGE_RAM_BANK_SIZE>, 16> ramBanks;
    // A000-BFFF
    std::array<u8, CARTRIDGE_RAM_BANK_SIZE> & activeRamBank = ramBanks[0];

    // 0000-3FFF
    std::array<std::array<u8, CARTRIDGE_ROM_BANK_SIZE>, 512> romBanks;
    // 0000-7FFF
    std::array<u8, CARTRIDGE_ROM_BANK_SIZE> & activeRomBank1 = romBanks[1];
};
}
