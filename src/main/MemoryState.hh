#pragma once

#include <array>

#include "Common.hh"

namespace gb4e
{
class GbCpuState;
class GbGpuState;

class MemoryState
{
public:
    virtual u8 Read(u16 location) const = 0;
    virtual u16 Read16(u16 location) const = 0;
    virtual void Write(u16 location, u8 value) = 0;
};

class MemoryStateFake : public MemoryState
{
public:
    u8 Read(u16 location) const final override;
    u16 Read16(u16 location) const final override;
    void Write(u16 location, u8 value) final override;

private:
    std::array<u8, MEMORY_SIZE> memory;
};

class GbMemoryState : public MemoryState
{
public:
    GbMemoryState(GbCpuState * cpu, GbGpuState * gpu) : cpu(cpu), gpu(gpu) {}

    u8 Read(u16 location) const final override;
    u16 Read16(u16 location) const final override;
    void Write(u16 location, u8 value) final override;

private:
    GbCpuState * cpu;
    GbGpuState * gpu;
};
}