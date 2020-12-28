#pragma once

#include <array>
#include <unordered_map>

#include "Common.hh"
#include "InstructionResult.hh"
#include "MemoryHandlers.hh"

size_t constexpr MEMORY_SIZE = 0x10000;

namespace gb4e
{
class GbCpu;
class Register;

class GbCpuState
{
public:
    friend class GbCpu;
    GbCpuState() = default;
    GbCpuState(size_t bootromSize, u8 const * bootrom);

    u8 Get8BitRegisterValue(Register const * reg) const;
    u16 Get16BitRegisterValue(Register const * reg) const;
    u8 Get8BitMemoryValue(u16 location) const;
    u16 Get16BitMemoryValue(u16 location) const;
    u8 GetFlags() const;

    void ApplyInstructionResult(InstructionResult const & result);
    void Set8BitRegisterValue(Register const * reg, u8 value);
    void Set16BitRegisterValue(Register const * reg, u16 value);
    void Set8BitMemoryValue(u16 location, u8 value);
    void SetFlags(u8 flags);

private:
    std::array<u16, 6> registers;
    std::array<u8, MEMORY_SIZE> memory{0};

    std::unordered_map<u16, MemoryReadHandler> memoryReadHandlers;
    std::unordered_map<u16, MemoryWriteHandler> memoryWriteHandlers;

    size_t bootromSize;
    u8 const * bootrom;
};
};