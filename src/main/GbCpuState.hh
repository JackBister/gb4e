#pragma once

#include <array>
#include <optional>
#include <unordered_map>

#include "Common.hh"
#include "InstructionResult.hh"
#include "MemoryHandlers.hh"
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
    /*
    u8 Get8BitMemoryValue(u16 location) const;
    u16 Get16BitMemoryValue(u16 location) const;
    */
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

private:
    std::array<u16, 6> registers;
    std::array<u8, MEMORY_SIZE> memory{0};

    // Set by EI
    bool hasPendingImeEnable = false;
    // Set by DI, RETI, INT
    bool ime = false;

    // FF00
    u8 joypSelect = 0;
    u8 buttons = 0x0F;
    u8 dpad = 0x0F;

    // FF0F
    u8 interruptFlags = 0;

    // FFFF
    u8 interruptEnable = 0;

    std::unordered_map<u16, MemoryWriteHandler> memoryWriteHandlers;

    size_t bootromSize;
    u8 const * bootrom;
    bool isBootromActive = true;
};
};