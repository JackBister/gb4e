#include "Cartridge.hh"

#include "romfile/RomFile.hh"

namespace gb4e
{

void Cartridge::LoadRom(RomFile const * romFile)
{
    this->romFile = romFile;
    this->activeRamBank = ramBanks[0];
    this->activeRomBank1 = romBanks[1];

    auto romSize = romFile->GetRomSize();

    for (int i = 0; i < romSize->GetNumBanks(); ++i) {
        // TODO: This seems pretty unsafe
        memcpy(&romBanks[i][0], romFile->GetData() + i * CARTRIDGE_ROM_BANK_SIZE, CARTRIDGE_ROM_BANK_SIZE);
    }
}

bool Cartridge::WriteMemory(u16 addr, u8 val)
{
    if (addr >= 0xA000 && addr <= 0xBFFF) {
        activeRamBank[addr - 0xA000] = val;
        return true;
    }
    return false;
}

std::optional<u8> Cartridge::ReadMemory(u16 addr) const
{
    if (addr <= 0x3FFF) {
        return romBanks[0][addr];
    }
    if (addr <= 0x7FFF) {
        return activeRomBank1[addr - 0x4000];
    }
    if (addr >= 0xA000 && addr <= 0xBFFF) {
        return activeRamBank[addr - 0xA000];
    }

    return {};
}
}
