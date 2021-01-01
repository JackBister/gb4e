#include "MemoryState.hh"

#include <cassert>

#include "GbCpuState.hh"
#include "GbGpuState.hh"
#include "audio/GbApuState.hh"

namespace gb4e
{

u8 MemoryStateFake::Read(u16 location) const
{
    return memory[location];
}

u16 MemoryStateFake::Read16(u16 location) const
{
    assert(location < 0xFFFF);
    return ((u16)memory[location]) | ((u16)memory[location + 1] << 8);
}

void MemoryStateFake::Write(u16 location, u8 value)
{
    memory[location] = value;
}

u8 GbMemoryState::Read(u16 location) const
{
    auto gpuValue = gpu->ReadMemory(location);
    if (gpuValue.has_value()) {
        return gpuValue.value();
    }
    auto apuValue = apu->ReadMemory(location);
    if (apuValue.has_value()) {
        return apuValue.value();
    }
    auto cpuValue = cpu->ReadMemory(location);
    if (cpuValue.has_value()) {
        return cpuValue.value();
    }
    assert(false);
    return 0xCD;
}

u16 GbMemoryState::Read16(u16 location) const
{
    assert(location < 0xFFFF);
    return ((u16)Read(location)) | ((u16)Read(location + 1) << 8);
}

void GbMemoryState::Write(u16 location, u8 value)
{
    cpu->WriteMemory(location, value);
    gpu->WriteMemory(location, value);
    apu->WriteMemory(location, value);
}
}
