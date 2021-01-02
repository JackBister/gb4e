#include "InstructionAppliers.hh"

#include "GbCpuState.hh"
#include "MemoryState.hh"

namespace gb4e
{

InstructionResult CallA16(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr pc(RegisterName::PC);
    Register constexpr sp(RegisterName::SP);

    u16 prevSpAddr = state->Get16BitRegisterValue(sp);
    u8 prevHi = memory->Read(prevSpAddr - 1);
    u8 prevLo = memory->Read(prevSpAddr - 2);

    u16 prevPc = state->Get16BitRegisterValue(pc);
    u16 srcValue = prevPc + 3;
    u8 newHi = (srcValue >> 8) & 0xFF;
    u8 newLo = srcValue & 0xFF;

    u16 newPc = memory->Read16(prevPc + 1);

    return InstructionResult(
        {MemoryWrite(prevSpAddr - 1, prevHi, newHi), MemoryWrite(prevSpAddr - 2, prevLo, newLo)},
        {RegisterWrite(sp, prevSpAddr, prevSpAddr - 2), RegisterWrite(pc, prevPc, newPc)},
        0, // TODO: Not sure I like this, this prevents GbCpuState::ApplyInstructionResult from adding even more to
           // PC, since CALL should already put you at the final address. An alternative would be if all
           // instructions instead returned a RegisterWrite to PC, but that could get pretty messy too.
        6);
}

InstructionResult CpD8(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr pc(RegisterName::PC);
    Register constexpr cpReg(RegisterName::A);

    u16 pcValue = state->Get16BitRegisterValue(pc);
    assert(pcValue < 0xFFFF);

    u8 valueAtAddr = memory->Read(pcValue + 1);
    u8 valueInReg = state->Get8BitRegisterValue(cpReg);

    u8 prevFlags = state->GetFlags();
    u8 flags = FLAG_N; // Always set N flag
    if (valueAtAddr == valueInReg) {
        flags |= FLAG_ZERO;
    }
    return InstructionResult(FlagSet(prevFlags, flags), 2, 2);
}

InstructionResult JrS8(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr pc(RegisterName::PC);

    u16 pcPrevValue = state->Get16BitRegisterValue(pc);
    assert(pcPrevValue < 0xFFFF);
    s8 imm = memory->Read(pcPrevValue + 1);
    return InstructionResult(RegisterWrite(pc, pcPrevValue, pcPrevValue + imm), 2, 3);
}

InstructionResult LdA8(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr pc(RegisterName::PC);
    Register constexpr srcReg(RegisterName::A);

    u16 pcValue = state->Get16BitRegisterValue(pc);
    assert(pcValue < 0xFFFF);
    u8 addrImm = memory->Read(pcValue + 1);
    u16 addr = 0xFF00 + addrImm;

    u8 prevValue = memory->Read(addr);
    u8 newValue = state->Get8BitRegisterValue(srcReg);

    return InstructionResult(MemoryWrite(addr, prevValue, newValue), 2, 3);
}

InstructionResult Ret(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr pc(RegisterName::PC);
    Register constexpr sp(RegisterName::SP);

    u16 prevSpAddr = state->Get16BitRegisterValue(sp);
    assert(prevSpAddr < 0xFFFF);
    u8 pcLo = memory->Read(prevSpAddr);
    u8 pcHi = memory->Read(prevSpAddr + 1);

    u16 prevPcValue = state->Get16BitRegisterValue(pc);
    u16 newPcValue = ((u16)pcLo) | (((u16)pcHi) << 8);

    return InstructionResult(
        // TODO: See CallA16 for why consumedBytes is 0
        {RegisterWrite(sp, prevSpAddr, prevSpAddr + 2), RegisterWrite(pc, prevPcValue, newPcValue)},
        0,
        4);
}

InstructionResult Rla(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr reg(RegisterName::A);

    u8 prevValue = state->Get8BitRegisterValue(reg);
    u8 prevFlags = state->GetFlags();
    u8 prevCarry = (prevFlags >> 4) & 0b00000001;
    u8 newCarry = (prevValue >> 7) & 0b00000001;
    u8 newFlags = newCarry ? FLAG_C : 0;

    u8 newValue = (prevValue << 1) | prevCarry;
    return InstructionResult(FlagSet(prevFlags, newFlags), RegisterWrite(reg, prevValue, newValue), 1, 1);
}

};
