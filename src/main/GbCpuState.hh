#pragma once

#include <array>
#include <optional>
#include <unordered_map>

#include "Common.hh"
#include "InstructionResult.hh"
#include "MemoryState.hh"

namespace gb4e
{
class GbCpu;
class GbGpuState;
class Register;

class GbCpuState final
{
public:
    friend class GbCpu;
    GbCpuState() = default;
    GbCpuState(size_t bootromSize, u8 const * bootrom);

    void Reset();

    u8 Get8BitRegisterValue(Register const * reg) const;
    u16 Get16BitRegisterValue(Register const * reg) const;

    u8 Get8BitRegisterValue(Register const reg) const;
    u16 Get16BitRegisterValue(Register const reg) const;

    std::optional<u8> ReadMemory(u16 location) const;

    u8 GetFlags() const;
    u8 GetInterruptEnable() const { return interruptEnable; }
    bool GetInterruptFlags() const { return interruptFlags; }
    bool GetInterruptMasterEnable() const { return ime; }
    bool HasPendingImeEnable() const { return hasPendingImeEnable; }

    void Set8BitRegisterValue(Register const * reg, u8 value);
    void Set16BitRegisterValue(Register const * reg, u16 value);
    void Set8BitRegisterValue(Register const reg, u8 value);
    void Set16BitRegisterValue(Register const reg, u16 value);

    bool WriteMemory(u16 location, u8 value);
    void SetFlags(u8 flags);
    void SetInterruptMasterEnable(bool enabled) { this->ime = enabled; }
    void EnableInterruptsWithDelay() { this->hasPendingImeEnable = true; }

    void SetOamDmaLocation(u16 oamDmaLocation) { this->oamDmaLocation = oamDmaLocation; }
    u16 GetOamDmaLocation() const { return this->oamDmaLocation; }

private:
    std::array<u16, 6> registers;
    std::array<u8, MEMORY_SIZE> memory{0};

    // Set by EI
    bool hasPendingImeEnable = false;
    // Set by DI, RETI, INT
    bool ime = false;

    // FF0F
    u8 interruptFlags = 0;

    // FF46
    u16 oamDmaLocation = 0xFFFF;

    // FFFF
    u8 interruptEnable = 0;

    size_t bootromSize;
    u8 const * bootrom;
    bool isBootromActive = true;
};
};