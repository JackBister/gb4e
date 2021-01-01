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
/*
u8 GbCpuState::Get8BitMemoryValue(u16 location) const
{
    return memory[location];
}

u16 GbCpuState::Get16BitMemoryValue(u16 location) const
{
    assert(location < 0xFFFF);
    return ((u16)Get8BitMemoryValue(location)) | ((u16)Get8BitMemoryValue(location + 1) << 8);
}
*/

std::optional<u8> GbCpuState::ReadMemory(u16 location) const
{
    if (isBootromActive && location < bootromSize) {
        return bootrom[location];
    }
    if (location >= 0x8000 && location <= 0x9FFF) {
        return {};
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

/*
void GbCpuState::Set8BitMemoryValue(u16 location, u8 value)
{
    if (memoryWriteHandlers.find(location) != memoryWriteHandlers.end()) {
        bool allowWrite = memoryWriteHandlers.at(location)(this, location, value);
        if (allowWrite) {
            memory[location] = value;
        } else {
            logger->Infof(
                "Attempt to write value=%02x to location=%04x was prevented by memory write handler", value, location);
        }
    } else {
        memory[location] = value;
    }
}
*/

bool GbCpuState::WriteMemory(u16 location, u8 value)
{
    if (location == 0xFF50 && value) {
        isBootromActive = false;
        memory[location] = value;
        return true;
    }
    if (memoryWriteHandlers.find(location) != memoryWriteHandlers.end()) {
        bool allowWrite = memoryWriteHandlers.at(location)(this, location, value);
        if (allowWrite) {
            memory[location] = value;
        } else {
            logger->Infof(
                "Attempt to write value=%02x to location=%04x was prevented by memory write handler", value, location);
        }
    } else {
        memory[location] = value;
    }
    return true;
}

void GbCpuState::SetFlags(u8 flags)
{
    registers[GetRegister(RegisterName::AF)->GetIndex()] &= 0xFF00;
    registers[GetRegister(RegisterName::AF)->GetIndex()] |= flags;
}

};