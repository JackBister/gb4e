#include "InstructionAppliers.hh"

#include "GbCpuState.hh"
#include "MemoryState.hh"
#include "logging/Logger.hh"

static auto const logger = Logger::Create("InstructionAppliers");

namespace gb4e
{

InstructionResult AdcD8(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr pc(RegisterName::PC);
    Register constexpr a(RegisterName::A);

    u16 pcAddr = state->Get16BitRegisterValue(pc);
    assert(pcAddr < 0xFFFF);

    u8 d8 = memory->Read(pcAddr + 1);

    u8 prevFlags = state->GetFlags();
    u8 prevValue = state->Get8BitRegisterValue(a);
    u8 carry = (prevFlags & FLAG_C) ? 1 : 0;

    u16 newValue = prevValue + d8 + carry;

    u8 newFlags = 0;
    if (((u8)newValue) == 0) {
        newFlags |= FLAG_ZERO;
    }

    if (((u8)prevValue ^ d8 ^ newValue) & BIT(4)) { // What the fuck?
        newFlags |= FLAG_HC;
    }

    if (newValue & BIT(8)) {
        newFlags |= FLAG_C;
    }

    return InstructionResult(FlagSet(prevFlags, newFlags), RegisterWrite(a, prevValue, (u8)newValue), 2, 2);
}

InstructionResult AndD8(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr pc(RegisterName::PC);
    Register constexpr a(RegisterName::A);

    u16 pcAddr = state->Get16BitRegisterValue(pc);
    assert(pcAddr < 0xFFFF);

    u8 d8 = memory->Read(pcAddr + 1);

    u8 prevValue = state->Get8BitRegisterValue(a);
    u8 newValue = prevValue & d8;

    u8 prevFlags = state->GetFlags();
    u8 flags = 0b00100000;
    if (newValue == 0) {
        flags |= FLAG_ZERO;
    }

    return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(a, prevValue, newValue), 2, 2);
}

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
    Register constexpr dstReg(RegisterName::A);
    u16 pcValue = state->Get16BitRegisterValue(pc);
    u8 srcValue = memory->Read(pcValue + 1);
    u8 dstValue = state->Get8BitRegisterValue(dstReg);
    u8 result = dstValue - srcValue;

    u8 prevFlags = state->GetFlags();
    u8 newFlags = FLAG_N;
    if (result == 0) {
        newFlags |= FLAG_ZERO;
    }
    if (((int)(dstValue & 0xF)) - ((int)(srcValue & 0xF)) < 0) {
        newFlags |= FLAG_HC;
    }
    if (((int)(dstValue)) - ((int)(srcValue)) < 0) {
        newFlags |= FLAG_C;
    }
    return InstructionResult(FlagSet(prevFlags, newFlags), 2, 2);
}

InstructionResult Cpl(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr a(RegisterName::A);

    u8 prevValue = state->Get8BitRegisterValue(a);
    u8 newValue = ~prevValue;

    u8 prevFlags = state->GetFlags();
    u8 newFlags = (prevFlags & 0b10010000) | 0b01100000;

    return InstructionResult(FlagSet(prevFlags, newFlags), RegisterWrite(a, prevValue, newValue), 1, 1);
}

InstructionResult Daa(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr a(RegisterName::A);
    u8 prevFlags = state->GetFlags();
    u8 prevValue = state->Get8BitRegisterValue(a);
    u8 newValue = prevValue;

    u8 lo = prevValue & 0x0F;
    u8 hi = prevValue & 0xF0;

    u8 correction = 0;
    u8 cFlag = 0;
    if ((prevFlags & FLAG_HC) || (lo > 9)) {
        correction = 0x06;
    }

    if ((prevFlags & FLAG_C) || (hi > 9)) {
        correction += 0x60;
        cFlag |= FLAG_C;
    }

    if (prevFlags & FLAG_N) {
        newValue -= correction;
    } else {
        newValue += correction;
    }

    u8 newFlags = 0;
    newFlags |= (newValue == 0) ? FLAG_ZERO : 0;
    newFlags |= (prevFlags & FLAG_N);
    newFlags |= cFlag;
    return InstructionResult(FlagSet(prevFlags, newFlags), RegisterWrite(a, prevValue, newValue), 1, 1);
}

InstructionResult DecHlAddr(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr reg(RegisterName::HL);
    u16 hl = state->Get16BitRegisterValue(reg);
    u8 prevValue = memory->Read(hl);
    u8 newValue = prevValue - 1;
    u8 prevFlags = state->GetFlags();
    u8 flags = 0;
    flags |= prevFlags & FLAG_C; // Keep carry flag
    flags |= FLAG_N;             // Always set sub flag
    if (newValue == 0) {
        flags |= FLAG_ZERO;
    }
    if (newValue == 0b00001111) {
        flags |= FLAG_HC;
    }
    return InstructionResult(FlagSet(prevFlags, flags), MemoryWrite(hl, prevValue, newValue), 1, 3);
}

InstructionResult IncHlAddr(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr reg(RegisterName::HL);
    u16 hl = state->Get16BitRegisterValue(reg);
    u8 prevValue = memory->Read(hl);
    u8 newValue = prevValue + 1;
    u8 prevFlags = state->GetFlags();
    u8 flags = 0;
    flags |= prevFlags & FLAG_C; // Keep carry flag
    if (newValue == 0) {
        flags |= FLAG_ZERO;
    }
    if (prevValue == 0b00001111) {
        flags |= FLAG_HC; // half carry flag
    }
    return InstructionResult(FlagSet(prevFlags, flags), MemoryWrite(hl, prevValue, newValue), 1, 3);
}

InstructionResult JpA16(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr pc(RegisterName::PC);

    u16 pcValue = state->Get16BitRegisterValue(pc);
    assert(pcValue < 0xFFFE);

    u16 newPc = memory->Read16(pcValue + 1);

    return InstructionResult(RegisterWrite(pc, pcValue, newPc), 0, 4);
}

InstructionResult JpHl(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr pc(RegisterName::PC);
    Register constexpr hl(RegisterName::HL);

    u16 prevValue = state->Get16BitRegisterValue(pc);
    u16 newValue = state->Get16BitRegisterValue(hl);

    return InstructionResult(RegisterWrite(pc, prevValue, newValue), 0, 1);
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

InstructionResult LdHlD8(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr hl(RegisterName::HL);
    Register constexpr pc(RegisterName::PC);

    u16 hlValue = state->Get16BitRegisterValue(hl);
    u16 pcValue = state->Get16BitRegisterValue(pc);

    u8 d8 = memory->Read(pcValue + 1);
    u8 prevValue = memory->Read(hlValue);
    return InstructionResult(MemoryWrite(hlValue, prevValue, d8), 2, 3);
}

InstructionResult LdHlSpPlusImm(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr hl(RegisterName::HL);
    Register constexpr pc(RegisterName::PC);
    Register constexpr sp(RegisterName::SP);

    u16 prevValue = state->Get16BitRegisterValue(hl);
    u16 pcValue = state->Get16BitRegisterValue(pc);
    u8 imm = memory->Read(pcValue + 1);
    s8 immS8 = (s8)imm;
    u16 spValue = state->Get16BitRegisterValue(sp);
    u16 newValue = spValue + immS8;

    u8 prevFlags = state->GetFlags();
    u8 newFlags = 0;
    if (immS8 >= 0) {
        newFlags |= (((spValue & 0xFF) + immS8) > 0xFF) ? FLAG_C : 0;
        newFlags |= (((spValue & 0xF) + (immS8 & 0xF)) > 0xF) ? FLAG_HC : 0;
    } else {
        newFlags |= ((spValue & 0xFF) <= (newValue & 0xFF)) ? FLAG_C : 0;
        newFlags |= ((spValue & 0xF) <= (newValue & 0xF)) ? FLAG_HC : 0;
    }

    return InstructionResult(FlagSet(prevFlags, newFlags), RegisterWrite(hl, prevValue, newValue), 2, 3);
}

InstructionResult LdSpHl(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr hl(RegisterName::HL);
    Register constexpr sp(RegisterName::SP);

    u16 prevValue = state->Get16BitRegisterValue(sp);
    u16 newValue = state->Get16BitRegisterValue(hl);
    return InstructionResult(RegisterWrite(sp, prevValue, newValue), 1, 2);
}

InstructionResult OrD8(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr dstReg(RegisterName::A);
    u8 prevValue = state->Get8BitRegisterValue(dstReg);
    u16 pcValue = state->Get16BitRegisterValue(Register(RegisterName::PC));
    u8 imm = memory->Read(pcValue + 1);
    u8 newValue = prevValue | imm;

    u8 prevFlags = state->GetFlags();
    u8 flags = 0;
    if (newValue == 0) {
        flags |= FLAG_ZERO;
    }
    return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(dstReg, prevValue, newValue), 2, 2);
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

InstructionResult Reti(GbCpuState const * state, MemoryState const * memory)
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
        InterruptSet(state->GetInterruptMasterEnable(), true),
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

InstructionResult Rlca(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr reg(RegisterName::A);

    u8 prevValue = state->Get8BitRegisterValue(reg);
    u8 prevFlags = state->GetFlags();
    u8 newCarry = (prevValue >> 7) & 0b00000001;
    u8 newFlags = newCarry ? FLAG_C : 0;

    u8 newValue = (prevValue << 1) | newCarry;
    return InstructionResult(FlagSet(prevFlags, newFlags), RegisterWrite(reg, prevValue, newValue), 1, 1);
}

InstructionResult Rra(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr reg(RegisterName::A);

    u8 prevValue = state->Get8BitRegisterValue(reg);
    u8 prevFlags = state->GetFlags();
    u8 prevCarry = (prevFlags >> 4) & 0b00000001;
    u8 newCarry = prevValue & 0b00000001;
    u8 newFlags = newCarry ? FLAG_C : 0;

    u8 newValue = (prevValue >> 1) | (prevCarry << 7);
    return InstructionResult(FlagSet(prevFlags, newFlags), RegisterWrite(reg, prevValue, newValue), 1, 1);
}

InstructionResult SbcD8(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr dstReg(RegisterName::A);
    Register constexpr pcReg(RegisterName::PC);

    u16 pcValue = state->Get16BitRegisterValue(pcReg);
    u8 imm = memory->Read(pcValue + 1);

    u8 prevFlags = state->GetFlags();
    u8 prevValue = state->Get8BitRegisterValue(dstReg);
    u8 newValue = prevValue - imm - ((prevFlags & FLAG_C) ? 1 : 0);

    u8 flags = FLAG_N; // Always set sub flag
    if (newValue == 0) {
        flags |= FLAG_ZERO;
    }
    if (prevValue > 0b00010000 && newValue <= 0b00010000) {
        flags |= FLAG_HC;
    }
    if (newValue >= prevValue && imm != 0) {
        flags |= FLAG_C;
    }
    return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(dstReg, prevValue, newValue), 2, 2);
}

InstructionResult SubD8(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr dstReg(RegisterName::A);
    Register constexpr pcReg(RegisterName::PC);

    u16 pcValue = state->Get16BitRegisterValue(pcReg);
    u8 imm = memory->Read(pcValue + 1);

    u8 prevValue = state->Get8BitRegisterValue(dstReg);
    u8 newValue = prevValue - imm;

    u8 prevFlags = state->GetFlags();
    u8 flags = FLAG_N; // Always set sub flag
    if (newValue == 0) {
        flags |= FLAG_ZERO;
    }
    if (prevValue > 0b00010000 && newValue <= 0b00010000) {
        flags |= FLAG_HC;
    }
    if (newValue >= prevValue && imm != 0) {
        flags |= FLAG_C;
    }
    return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(dstReg, prevValue, newValue), 2, 2);
}

InstructionResult XorD8(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr dstReg(RegisterName::A);
    Register constexpr pcReg(RegisterName::PC);

    u16 pcValue = state->Get16BitRegisterValue(pcReg);
    u8 imm = memory->Read(pcValue + 1);

    u8 prevValue = state->Get8BitRegisterValue(dstReg);
    u8 newValue = prevValue ^ imm;

    u8 prevFlags = state->GetFlags();
    u8 newFlags = 0;
    if (newValue == 0) {
        newFlags |= FLAG_ZERO;
    }

    return InstructionResult(FlagSet(prevFlags, newFlags), RegisterWrite(dstReg, prevValue, newValue), 2, 2);
}
};
