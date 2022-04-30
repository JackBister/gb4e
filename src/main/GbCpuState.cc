#include "GbCpuState.hh"

#include <cassert>

#include "Register.hh"
#include "logging/Logger.hh"

static auto const logger = Logger::Create("GbCpuState");

namespace gb4e
{

GbCpuState::GbCpuState(size_t bootromSize, u8 const * bootrom) : bootromSize(bootromSize), bootrom(bootrom)
{
    Set16BitRegisterValue(GetRegister(RegisterName::AF), 0x01B0);
    Set16BitRegisterValue(GetRegister(RegisterName::BC), 0x0013);
    Set16BitRegisterValue(GetRegister(RegisterName::DE), 0x00D8);
    Set16BitRegisterValue(GetRegister(RegisterName::HL), 0x014D);
    Set16BitRegisterValue(GetRegister(RegisterName::SP), 0xFFFE);
    Set16BitRegisterValue(GetRegister(RegisterName::PC), 0x0000);
}

void GbCpuState::Reset()
{
    Set16BitRegisterValue(GetRegister(RegisterName::AF), 0x01B0);
    Set16BitRegisterValue(GetRegister(RegisterName::BC), 0x0013);
    Set16BitRegisterValue(GetRegister(RegisterName::DE), 0x00D8);
    Set16BitRegisterValue(GetRegister(RegisterName::HL), 0x014D);
    Set16BitRegisterValue(GetRegister(RegisterName::SP), 0xFFFE);
    Set16BitRegisterValue(GetRegister(RegisterName::PC), 0x0000);
    isBootromActive = true;
}

u8 GbCpuState::Get8BitRegisterValue(Register const * reg) const
{
    assert(reg->Is8Bit());
    return ((u8 *)&registers[0])[reg->GetIndex()];
}

u16 GbCpuState::Get16BitRegisterValue(Register const * reg) const
{
    assert(reg->Is16Bit());
    return registers[reg->GetIndex()];
}

u8 GbCpuState::Get8BitRegisterValue(Register const reg) const
{
    assert(reg.Is8Bit());
    return ((u8 *)&registers[0])[reg.GetIndex()];
}

u16 GbCpuState::Get16BitRegisterValue(Register const reg) const
{
    assert(reg.Is16Bit());
    return registers[reg.GetIndex()];
}

std::optional<u8> GbCpuState::ReadMemory(u16 location) const
{
    if (isBootromActive && location < bootromSize) {
        return bootrom[location];
    }
    if (location < 0x7FFF) {
        return {};
    }
    if (location >= 0x8000 && location <= 0x9FFF) {
        return {};
    }
    if (location >= 0xA000 && location <= 0xBFFF) {
        return {};
    }
    if (location == 0xFF0F) {
        return interruptFlags;
    }
    if (location == 0xFFFF) {
        return interruptEnable;
    }
    return memory[location];
}

u8 GbCpuState::GetFlags() const
{
    u16 af = registers[GetRegister(RegisterName::AF)->GetIndex()];
    return af & 0xFF;
}

void GbCpuState::Set8BitRegisterValue(Register const * reg, u8 value)
{
    assert(reg->Is8Bit());
    ((u8 *)&registers[0])[reg->GetIndex()] = value;
}

void GbCpuState::Set16BitRegisterValue(Register const * reg, u16 value)
{
    assert(reg->Is16Bit());
    registers[reg->GetIndex()] = value;
}

void GbCpuState::Set8BitRegisterValue(Register const reg, u8 value)
{
    assert(reg.Is8Bit());
    ((u8 *)&registers[0])[reg.GetIndex()] = value;
}

void GbCpuState::Set16BitRegisterValue(Register const reg, u16 value)
{
    assert(reg.Is16Bit());
    registers[reg.GetIndex()] = value;
}

bool GbCpuState::WriteMemory(u16 location, u8 value)
{
    if (location == 0xFF46) {
        oamDmaLocation = value << 8;
        return true;
    }
    if (location == 0xFF50 && value) {
        isBootromActive = false;
        memory[location] = value;
        return true;
    }
    if (location == 0xFF0F) {
        interruptFlags = value;
        return true;
    }
    if (location == 0xFFFF) {
        interruptEnable = value;
        return true;
    }
    memory[location] = value;
    return true;
}

void GbCpuState::SetFlags(u8 flags)
{
    registers[GetRegister(RegisterName::AF)->GetIndex()] &= 0xFF00;
    registers[GetRegister(RegisterName::AF)->GetIndex()] |= flags;
}

};