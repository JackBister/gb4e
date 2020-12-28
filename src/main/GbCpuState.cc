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

    memoryWriteHandlers[0xFF11] = StubMemoryWriteHandler("Channel 1 Sound length/Wave pattern duty");
    memoryWriteHandlers[0xFF12] = StubMemoryWriteHandler("Channel 1 Volume Envelope");
    memoryWriteHandlers[0xFF24] = StubMemoryWriteHandler("Channel control / ON-OFF / Volume");
    memoryWriteHandlers[0xFF25] = StubMemoryWriteHandler("Selection of Sound output terminal");
    memoryWriteHandlers[0xFF26] = StubMemoryWriteHandler("Sound on/off");
    memoryWriteHandlers[0xFF40] = StubMemoryWriteHandler("LCD Control Register");
    memoryWriteHandlers[0xFF41] = StubMemoryWriteHandler("STAT - LCDC Status");
    memoryWriteHandlers[0xFF47] = StubMemoryWriteHandler("BG Palette Data");

    for (u16 i = 0xE000; i <= 0xFDFF; ++i) {
        // Echo from/to 0xC000~0xDDFF
        memoryReadHandlers[i] = EchoRamReadHandler(-8192);
        memoryWriteHandlers[i] = EchoRamWriteHandler(-8192);
    }

    // Setup FEAO-FEFF "prohibited" area
    for (u16 i = 0xFEA0; i <= 0xFEFF; ++i) {
        memoryWriteHandlers[i] = ReadonlyWriteHandler();
        // This is "GBC Revision E" behavior
        // TODO: Return FF during OAM block
        memory[i] = ((i & 0x00F0) >> 4) | (i & 0x00F0);
    }
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

u8 GbCpuState::Get8BitMemoryValue(u16 location) const
{
    if (memoryReadHandlers.find(location) != memoryReadHandlers.end()) {
        return memoryReadHandlers.at(location)(this, location);
    } else {
        return memory[location];
    }
}

u16 GbCpuState::Get16BitMemoryValue(u16 location) const
{
    assert(location < 0xFFFF);
    return ((u16)Get8BitMemoryValue(location)) | ((u16)Get8BitMemoryValue(location + 1) << 8);
}

u8 GbCpuState::GetFlags() const
{
    u16 af = registers[GetRegister(RegisterName::AF)->GetIndex()];
    return af & 0xFF;
}

void GbCpuState::ApplyInstructionResult(InstructionResult const & result)
{
    logger->Tracef("Applying instructionResult=%s", result.ToString().c_str());

    if (result.GetFlagSet().has_value()) {
        logger->Tracef("Setting flags=%02x", result.GetFlagSet().value().GetValue(), GetFlags());
        SetFlags(result.GetFlagSet().value().GetValue());
    }

    for (auto const & memWrite : result.GetMemoryWrites()) {
        logger->Tracef("Setting memLocation=%04x to value=%02x", memWrite.GetLocation(), memWrite.GetValue());
        Set8BitMemoryValue(memWrite.GetLocation(), memWrite.GetValue());
    }

    for (auto const & regWrite : result.GetRegisterWrites()) {
        if (regWrite.GetRegister()->Is8Bit()) {
            logger->Tracef("Setting 8-bit register=%s to value=%02x",
                           regWrite.GetRegister()->ToString().c_str(),
                           regWrite.GetByteValue());
            Set8BitRegisterValue(regWrite.GetRegister(), regWrite.GetByteValue());
        } else {
            logger->Tracef("Setting 16-bit register=%s to value=%02x",
                           regWrite.GetRegister()->ToString().c_str(),
                           regWrite.GetWordValue());
            Set16BitRegisterValue(regWrite.GetRegister(), regWrite.GetWordValue());
        }
    }

    auto pcReg = GetRegister(RegisterName::PC);
    u16 pc = Get16BitRegisterValue(GetRegister(RegisterName::PC));
    pc += result.GetConsumedBytes();
    logger->Tracef("Setting pc to newPcValue=%04x", pc);
    Set16BitRegisterValue(pcReg, pc);
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

void GbCpuState::SetFlags(u8 flags)
{
    registers[GetRegister(RegisterName::AF)->GetIndex()] &= 0xFF00;
    registers[GetRegister(RegisterName::AF)->GetIndex()] |= flags;
}

};