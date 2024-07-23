#pragma once

#include "GbCpuState.hh"
#include "Instruction.hh"

namespace gb4e
{

template <RegisterName SRC>
InstructionResult Adc(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr dst(RegisterName::A);
    Register constexpr src(SRC);
    u8 srcValue;
    if constexpr (src.Is16Bit()) {
        static_assert(SRC == RegisterName::HL);
        u16 hl = state->Get16BitRegisterValue(src);
        srcValue = memory->Read(hl);
    } else {
        srcValue = state->Get8BitRegisterValue(src);
    }

    u8 prevFlags = state->GetFlags();
    u8 prevValue = state->Get8BitRegisterValue(dst);
    u8 carry = (prevFlags & FLAG_C) ? 1 : 0;

    u16 newValue = prevValue + srcValue + carry;

    u8 newFlags = 0;
    if (((u8)newValue) == 0) {
        newFlags |= FLAG_ZERO;
    }

    if (((u8)prevValue ^ srcValue ^ newValue) & BIT(4)) { // What the fuck?
        newFlags |= FLAG_HC;
    }

    if (newValue & BIT(8)) {
        newFlags |= FLAG_C;
    }

    return InstructionResult(FlagSet(prevFlags, newFlags), RegisterWrite(dst, prevValue, (u8)newValue), 1, 2);
}

InstructionResult AdcD8(GbCpuState const * state, MemoryState const * memory);

template <RegisterName DST, RegisterName SRC>
InstructionResult Add(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr dstReg(DST);
    Register constexpr srcReg(SRC);

    if constexpr (dstReg.Is8Bit()) {
        static_assert(dstReg.GetRegisterName() == RegisterName::A, "ADD r8, r8 destination register must be A");
        static_assert(srcReg.Is8Bit(), "ADD r8, r8 source register must be 8-bit");
        u8 prevValue = state->Get8BitRegisterValue(dstReg);
        u8 add = state->Get8BitRegisterValue(srcReg);
        u8 newValue = prevValue + add;

        u8 prevFlags = state->GetFlags();
        u8 flags = 0;
        if (newValue == 0) {
            flags |= FLAG_ZERO;
        }
        if (prevValue < 0b00010000 && newValue >= 0b00010000) {
            flags |= FLAG_HC;
        }
        if (newValue <= prevValue && add != 0) {
            flags |= FLAG_C;
        }
        return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(dstReg, prevValue, newValue), 1, 1);
    } else {
        static_assert(dstReg.GetRegisterName() == RegisterName::HL, "ADD r16, r16 destination register must be HL");
        static_assert(srcReg.Is16Bit(), "ADD r16, r16 source register must be 16-bit");
        u16 prevValue = state->Get16BitRegisterValue(dstReg);
        u16 add = state->Get16BitRegisterValue(srcReg);

        u16 prevLo = prevValue & 0xFF;
        u16 loAdd = add & 0xFF;
        u16 newLo = prevLo + loAdd;
        u16 loCarry = (newLo & BIT(8)) >> 8;

        u16 prevHi = prevValue >> 8;
        u16 hiAdd = add >> 8;
        u16 newHi = prevHi + hiAdd + loCarry;

        u16 newValue = (newHi & 0xFF) << 8 | (newLo & 0xFF);

        u8 prevFlags = state->GetFlags();
        u8 flags = prevFlags & FLAG_ZERO; // Preserve zero flag
        if (((u8)prevHi ^ hiAdd ^ newHi) & BIT(4)) {
            flags |= FLAG_HC;
        }
        if (newHi & BIT(8)) {
            flags |= FLAG_C;
        }
        return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(dstReg, prevValue, newValue), 1, 2);
    }
}

template <RegisterName DST>
InstructionResult AddD8(GbCpuState const * state, MemoryState const * memory)
{
    static_assert(DST == RegisterName::A);
    Register constexpr dstReg(DST);
    Register constexpr pc(RegisterName::PC);

    u16 pcAddr = state->Get16BitRegisterValue(pc);
    assert(pcAddr < 0xFFFF);

    u8 d8 = memory->Read(pcAddr + 1);

    u8 prevValue = state->Get8BitRegisterValue(dstReg);
    u8 newValue = prevValue + d8;

    u8 prevFlags = state->GetFlags();
    u8 newFlags = 0;
    if (newValue == 0) {
        newFlags |= FLAG_ZERO;
    }
    if (prevValue < 0b00010000 && newValue >= 0b00010000) {
        newFlags |= FLAG_HC;
    }
    if (newValue <= prevValue && d8 != 0) {
        newFlags |= FLAG_C;
    }
    return InstructionResult(FlagSet(prevFlags, newFlags), RegisterWrite(dstReg, prevValue, newValue), 2, 2);
}

template <RegisterName DST, RegisterName SRC>
InstructionResult AddFromAddrReg(GbCpuState const * state, MemoryState const * memory)
{
    static_assert(DST == RegisterName::A, "Add r8, (r16) destination must be A");
    static_assert(SRC == RegisterName::HL, "Add r8, (r16) destination must be A");
    Register constexpr dstReg(DST);
    Register constexpr addrReg(SRC);

    u8 prevValue = state->Get8BitRegisterValue(dstReg);
    u16 addr = state->Get16BitRegisterValue(addrReg);
    u8 add = memory->Read(addr);

    u8 newValue = prevValue + add;

    u8 prevFlags = state->GetFlags();
    u8 flags = 0;
    if (newValue == 0) {
        flags |= FLAG_ZERO;
    }
    if (prevValue < 0b00010000 && newValue >= 0b00010000) {
        flags |= FLAG_HC;
    }
    if (newValue <= prevValue && add != 0) {
        flags |= FLAG_C;
    }
    return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(dstReg, prevValue, newValue), 1, 2);
}

template <RegisterName SRC>
InstructionResult And(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr srcReg(SRC);
    assert(srcReg.Is8Bit());
    Register constexpr dstReg(RegisterName::A);
    u8 prevValue = state->Get8BitRegisterValue(dstReg);
    u8 andV = state->Get8BitRegisterValue(srcReg);
    u8 newValue = prevValue & andV;

    u8 prevFlags = state->GetFlags();
    u8 flags = FLAG_HC; // Always set HC flag
    if (newValue == 0) {
        flags |= FLAG_ZERO;
    }
    return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(dstReg, prevValue, newValue), 1, 1);
}

InstructionResult AndD8(GbCpuState const * state, MemoryState const * memory);

template <u8 BIT, RegisterName regName>
InstructionResult Bit(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr reg(regName);
    static_assert(reg.Is8Bit(), "BIT x, r8 register must be 8-bit");
    static_assert(BIT < 8, "BIT x, r8 x must be less than 8");
    u8 regValue = state->Get8BitRegisterValue(reg);
    u8 value = (regValue >> BIT) & 1;

    u8 prevFlags = state->GetFlags();
    u8 flags = prevFlags & FLAG_C; // Preserve carry flag
    flags |= FLAG_HC;              // Always set HC flag
    if (value == 0) {
        flags |= FLAG_ZERO;
    }
    return InstructionResult(FlagSet(prevFlags, flags), 2, 2);
}

InstructionResult CallA16(GbCpuState const * state, MemoryState const * memory);

template <u8 FLAG, bool COND>
InstructionResult CallConditionalA16(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr pc(RegisterName::PC);
    Register constexpr sp(RegisterName::SP);

    u8 flags = state->GetFlags();
    u8 cond = flags & FLAG;
    if ((COND && cond) || (!COND && !cond)) {
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
    } else {
        return InstructionResult(3, 3);
    }
}

InstructionResult CpD8(GbCpuState const * state, MemoryState const * memory);

template <RegisterName SRC>
InstructionResult Cp(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr srcReg(SRC);
    Register constexpr dstReg(RegisterName::A);
    if constexpr (srcReg.Is16Bit()) {
        u16 location = state->Get16BitRegisterValue(srcReg);
        u8 srcValue = memory->Read(location);
        u8 dstValue = state->Get8BitRegisterValue(dstReg);
        u8 result = dstValue - srcValue;

        u8 prevFlags = state->GetFlags();
        u8 newFlags = FLAG_N;
        if (result == 0) {
            newFlags |= FLAG_ZERO;
        }
        if (((int)dstValue & 0xF) - ((int)srcValue & 0xF) < 0) {
            newFlags |= FLAG_HC;
        }
        if (((int)dstValue) - ((int)srcValue) < 0) {
            newFlags |= FLAG_C;
        }
        return InstructionResult(FlagSet(prevFlags, newFlags), 1, 2);
    } else {
        u8 dstValue = state->Get8BitRegisterValue(dstReg);
        u8 srcValue = state->Get8BitRegisterValue(srcReg);
        u8 result = dstValue - srcValue;

        u8 prevFlags = state->GetFlags();
        u8 newFlags = FLAG_N;
        if (result == 0) {
            newFlags |= FLAG_ZERO;
        }
        if (((int)dstValue & 0xF) - ((int)srcValue & 0xF) < 0) {
            newFlags |= FLAG_HC;
        }
        if (((int)dstValue) - ((int)srcValue) < 0) {
            newFlags |= FLAG_C;
        }
        return InstructionResult(FlagSet(prevFlags, newFlags), 1, 1);
    }
}

InstructionResult Cpl(GbCpuState const * state, MemoryState const * memory);

InstructionResult Daa(GbCpuState const * state, MemoryState const * memory);

template <RegisterName REG>
InstructionResult Dec(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr reg(REG);
    if constexpr (reg.Is8Bit()) {
        u8 prevValue = state->Get8BitRegisterValue(reg);
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
        return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(reg, prevValue, newValue), 1, 1);
    } else {
        u16 prevValue = state->Get16BitRegisterValue(reg);
        u16 newValue = prevValue - 1;

        return InstructionResult(RegisterWrite(reg, prevValue, newValue), 1, 2);
    }
}

InstructionResult DecHlAddr(GbCpuState const * state, MemoryState const * memory);

template <RegisterName REG>
InstructionResult Inc(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr reg(REG);
    if constexpr (reg.Is8Bit()) {
        u8 prevValue = state->Get8BitRegisterValue(reg);
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
        return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(reg, prevValue, newValue), 1, 1);
    } else {
        u16 prevValue = state->Get16BitRegisterValue(reg);
        u16 newValue = prevValue + 1;

        return InstructionResult(RegisterWrite(reg, prevValue, newValue), 1, 2);
    }
}

InstructionResult IncHlAddr(GbCpuState const * state, MemoryState const * memory);

InstructionResult JpA16(GbCpuState const * state, MemoryState const * memory);

template <u8 FLAGMASK>
InstructionResult JpFlagA16(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr pc(RegisterName::PC);

    u8 flags = state->GetFlags();
    if (flags & FLAGMASK) {
        u16 pcPrevValue = state->Get16BitRegisterValue(pc);
        assert(pcPrevValue < 0xFFFE);
        u16 pcNewValue = memory->Read16(pcPrevValue + 1);
        return InstructionResult(RegisterWrite(pc, pcPrevValue, pcNewValue), 0, 4);
    } else {
        return InstructionResult(3, 3);
    }
}

template <u8 FLAGMASK>
InstructionResult JpNFlagA16(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr pc(RegisterName::PC);

    u8 flags = state->GetFlags();
    if (!(flags & FLAGMASK)) {
        u16 pcPrevValue = state->Get16BitRegisterValue(pc);
        assert(pcPrevValue < 0xFFFE);
        u16 pcNewValue = memory->Read16(pcPrevValue + 1);
        return InstructionResult(RegisterWrite(pc, pcPrevValue, pcNewValue), 0, 4);
    } else {
        return InstructionResult(3, 3);
    }
}

InstructionResult JpHl(GbCpuState const * state, MemoryState const * memory);

template <u8 FLAGMASK>
InstructionResult JrFlagS8(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr pc(RegisterName::PC);

    u8 flags = state->GetFlags();
    if (flags & FLAGMASK) {
        u16 pcPrevValue = state->Get16BitRegisterValue(pc);
        assert(pcPrevValue < 0xFFFF);
        s8 imm = memory->Read(pcPrevValue + 1);
        return InstructionResult(RegisterWrite(pc, pcPrevValue, pcPrevValue + imm), 2, 3);
    } else {
        return InstructionResult(2, 2);
    }
}

template <u8 FLAGMASK>
InstructionResult JrNFlagS8(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr pc(RegisterName::PC);

    u8 flags = state->GetFlags();
    if (!(flags & FLAGMASK)) {
        u16 pcPrevValue = state->Get16BitRegisterValue(pc);
        assert(pcPrevValue < 0xFFFF);
        s8 imm = memory->Read(pcPrevValue + 1);
        return InstructionResult(RegisterWrite(pc, pcPrevValue, pcPrevValue + imm), 2, 3);
    } else {
        return InstructionResult(2, 2);
    }
}

InstructionResult JrS8(GbCpuState const * state, MemoryState const * memory);

template <RegisterName DST, RegisterName SRC>
InstructionResult Ld(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr dstReg(DST);
    Register constexpr srcReg(SRC);
    static_assert(dstReg.Is8Bit() && srcReg.Is8Bit(), "LD r8, r8: Both registers must be 8-bit");

    u8 dstPrevValue = state->Get8BitRegisterValue(dstReg);
    u8 srcValue = state->Get8BitRegisterValue(srcReg);
    return InstructionResult(RegisterWrite(dstReg, dstPrevValue, srcValue), 1, 1);
}

InstructionResult LdA8(GbCpuState const * state, MemoryState const * memory);

template <RegisterName SRC>
InstructionResult LdA16(GbCpuState const * state, MemoryState const * memory)
{
    static_assert(SRC == RegisterName::A || SRC == RegisterName::SP);
    Register constexpr srcReg(SRC);
    Register constexpr pc(RegisterName::PC);
    if constexpr (srcReg.Is8Bit()) {
        u16 pcValue = state->Get16BitRegisterValue(pc);
        assert(pcValue < 0xFFFE);
        u16 addr = memory->Read16(pcValue + 1);

        u8 prevValue = memory->Read(addr);
        u8 newValue = state->Get8BitRegisterValue(srcReg);

        return InstructionResult(MemoryWrite(addr, prevValue, newValue), 3, 4);
    } else {
        u16 pcValue = state->Get16BitRegisterValue(pc);
        assert(pcValue < 0xFFFE);
        u16 addr = memory->Read16(pcValue + 1);

        u8 prevLo = memory->Read(addr);
        u8 prevHi = memory->Read(addr + 1);

        u16 srcValue = state->Get16BitRegisterValue(srcReg);
        u8 newLo = srcValue & 0xFF;
        u8 newHi = (srcValue >> 8) & 0xFF;
        return InstructionResult({MemoryWrite(addr, prevLo, newLo), MemoryWrite(addr + 1, prevHi, newHi)}, 3, 5);
    }
}

template <RegisterName DST>
InstructionResult LdD8(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr dstReg(DST);
    static_assert(dstReg.Is8Bit());
    Register constexpr pc(RegisterName::PC);

    u8 dstPrevValue = state->Get8BitRegisterValue(dstReg);
    u16 pcValue = state->Get16BitRegisterValue(pc);
    assert(pcValue < 0xFFFF);
    u8 imm = memory->Read(pcValue + 1);
    return InstructionResult(RegisterWrite(dstReg, dstPrevValue, imm), 2, 2);
}

template <RegisterName DST>
InstructionResult LdD16(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr dstReg(DST);
    static_assert(dstReg.Is16Bit());
    Register constexpr pc(RegisterName::PC);

    u16 dstPrevValue = state->Get16BitRegisterValue(dstReg);
    u16 pcValue = state->Get16BitRegisterValue(pc);
    assert(pcValue < 0xFFFE);
    u16 imm = memory->Read16(pcValue + 1);
    return InstructionResult(RegisterWrite(dstReg, dstPrevValue, imm), 3, 3);
}

template <RegisterName DST, RegisterName ADDR, int MODIFY = 0>
InstructionResult LdFromAddrReg(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr dstReg(DST);
    Register constexpr addrReg(ADDR);
    static_assert(dstReg.Is8Bit());
    static_assert(addrReg.Is16Bit());

    u8 prevValue = state->Get8BitRegisterValue(dstReg);
    u16 addrValue = state->Get16BitRegisterValue(addrReg);
    u8 valueAtAddr = memory->Read(addrValue);

    if constexpr (MODIFY != 0) {
        static_assert(ADDR == RegisterName::HL);
        return InstructionResult(
            {RegisterWrite(addrReg, addrValue, addrValue + MODIFY), RegisterWrite(dstReg, prevValue, valueAtAddr)},
            1,
            2);
    }
    return InstructionResult(RegisterWrite(dstReg, prevValue, valueAtAddr), 1, 2);
}

template <RegisterName DST>
InstructionResult LdFromA8(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr dstReg(DST);
    static_assert(dstReg.Is8Bit());
    Register constexpr pc(RegisterName::PC);

    u16 pcValue = state->Get16BitRegisterValue(pc);
    assert(pcValue < 0xFFFF);
    u8 addrImm = memory->Read(pcValue + 1);
    u16 addr = 0xFF00 + addrImm;

    u8 prevValue = state->Get8BitRegisterValue(dstReg);
    u8 newValue = memory->Read(addr);
    return InstructionResult(RegisterWrite(dstReg, prevValue, newValue), 2, 3);
}

template <RegisterName DST>
InstructionResult LdFromA16(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr dstReg(DST);
    static_assert(dstReg.Is8Bit());
    Register constexpr pc(RegisterName::PC);

    u16 pcValue = state->Get16BitRegisterValue(pc);
    assert(pcValue < 0xFFFE);
    u16 addr = memory->Read16(pcValue + 1);

    u8 prevValue = state->Get8BitRegisterValue(dstReg);
    u8 newValue = memory->Read(addr);
    return InstructionResult(RegisterWrite(dstReg, prevValue, newValue), 3, 4);
}

InstructionResult LdHlD8(GbCpuState const * state, MemoryState const * memory);

template <s8 MODIFIER>
InstructionResult LdHlIncDecA(GbCpuState const * state, MemoryState const * memory)
{
    static_assert(MODIFIER == 1 || MODIFIER == -1);
    Register constexpr hl(RegisterName::HL);
    Register constexpr a(RegisterName::A);

    u16 hlPrevValue = state->Get16BitRegisterValue(hl);
    u8 memPrevValue = memory->Read(hlPrevValue);
    u8 aValue = state->Get8BitRegisterValue(a);

    return InstructionResult(
        MemoryWrite(hlPrevValue, memPrevValue, aValue), RegisterWrite(hl, hlPrevValue, hlPrevValue + MODIFIER), 1, 2);
}

InstructionResult LdHlSpPlusImm(GbCpuState const * state, MemoryState const * memory);

template <RegisterName ADDR, RegisterName SRC>
InstructionResult LdMemViaReg(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr addrReg(ADDR);
    Register constexpr srcReg(SRC);
    static_assert(srcReg.Is8Bit());
    if constexpr (addrReg.Is8Bit()) {
        static_assert(ADDR == RegisterName::C);
        u8 addrOffset = state->Get8BitRegisterValue(addrReg);
        u16 addr = 0xFF00 + addrOffset;
        u8 srcValue = state->Get8BitRegisterValue(srcReg);
        u8 prevValue = memory->Read(addr);
        return InstructionResult(MemoryWrite(addr, prevValue, srcValue), 1, 2);
    } else {
        static_assert(ADDR == RegisterName::BC || ADDR == RegisterName::DE || ADDR == RegisterName::HL);
        u16 addr = state->Get16BitRegisterValue(addrReg);
        u8 srcValue = state->Get8BitRegisterValue(srcReg);
        u8 prevValue = memory->Read(addr);
        return InstructionResult(MemoryWrite(addr, prevValue, srcValue), 1, 2);
    }
}

InstructionResult LdSpHl(GbCpuState const * state, MemoryState const * memory);

template <RegisterName SRC>
InstructionResult Or(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr srcReg(SRC);
    Register constexpr dstReg(RegisterName::A);
    if constexpr (srcReg.Is16Bit()) {
        u16 location = state->Get16BitRegisterValue(srcReg);
        u8 prevValue = state->Get8BitRegisterValue(dstReg);
        u8 orV = memory->Read(location);
        u8 newValue = prevValue | orV;

        u8 prevFlags = state->GetFlags();
        u8 flags = 0;
        if (newValue == 0) {
            flags |= FLAG_ZERO;
        }
        return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(dstReg, prevValue, newValue), 1, 2);
    } else {
        u8 prevValue = state->Get8BitRegisterValue(dstReg);
        u8 orV = state->Get8BitRegisterValue(srcReg);
        u8 newValue = prevValue | orV;

        u8 prevFlags = state->GetFlags();
        u8 flags = 0;
        if (newValue == 0) {
            flags |= FLAG_ZERO;
        }
        return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(dstReg, prevValue, newValue), 1, 1);
    }
}

InstructionResult OrD8(GbCpuState const * state, MemoryState const * memory);

template <RegisterName DST>
InstructionResult Pop(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr dstReg(DST);
    static_assert(dstReg.Is16Bit());
    Register constexpr sp(RegisterName::SP);

    u16 prevSpAddr = state->Get16BitRegisterValue(sp);
    u16 prevValue = state->Get16BitRegisterValue(dstReg);

    u8 popLo = memory->Read(prevSpAddr);
    u8 popHi = memory->Read(prevSpAddr + 1);

    u16 newValue = ((u16)popLo) | (((u16)popHi) << 8);
    return InstructionResult(
        {RegisterWrite(sp, prevSpAddr, prevSpAddr + 2), RegisterWrite(dstReg, prevValue, newValue)}, 1, 3);
}

template <RegisterName SRC>
InstructionResult Push(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr srcReg(SRC);
    static_assert(srcReg.Is16Bit());
    Register constexpr sp(RegisterName::SP);

    u16 prevSpAddr = state->Get16BitRegisterValue(sp);
    u8 prevHi = memory->Read(prevSpAddr - 1);
    u8 prevLo = memory->Read(prevSpAddr - 2);

    u16 srcValue = state->Get16BitRegisterValue(srcReg);
    u8 newHi = (srcValue >> 8) & 0xFF;
    u8 newLo = srcValue & 0xFF;

    return InstructionResult({MemoryWrite(prevSpAddr - 1, prevHi, newHi), MemoryWrite(prevSpAddr - 2, prevLo, newLo)},
                             RegisterWrite(sp, prevSpAddr, prevSpAddr - 2),
                             1,
                             4);
}

template <u8 B, RegisterName REG>
InstructionResult Res(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr reg(REG);

    u8 mask = ~((u8)BIT(B));

    if constexpr (reg.Is16Bit()) {
        static_assert(REG == RegisterName::HL);
        u16 hl = state->Get16BitRegisterValue(reg);
        u8 prevValue = memory->Read(hl);
        u8 newValue = prevValue & mask;
        return InstructionResult(MemoryWrite(hl, prevValue, newValue), 2, 4);
    } else {
        u8 prevValue = state->Get8BitRegisterValue(reg);
        u8 newValue = prevValue & mask;
        return InstructionResult(RegisterWrite(reg, prevValue, newValue), 2, 2);
    }
}

InstructionResult Ret(GbCpuState const * state, MemoryState const * memory);

InstructionResult Reti(GbCpuState const * state, MemoryState const * memory);

template <u8 FLAGMASK>
InstructionResult RetFlag(GbCpuState const * state, MemoryState const * memory)
{
    u8 flags = state->GetFlags();
    if (flags & FLAGMASK) {
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
            5);
    } else {
        return InstructionResult(1, 2);
    }
}

template <u8 FLAGMASK>
InstructionResult RetNFlag(GbCpuState const * state, MemoryState const * memory)
{
    u8 flags = state->GetFlags();
    if (!(flags & FLAGMASK)) {
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
            5);
    } else {
        return InstructionResult(1, 2);
    }
}

template <RegisterName REG>
InstructionResult Rl(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr reg(REG);
    static_assert(reg.Is8Bit());
    u8 prevValue = state->Get8BitRegisterValue(reg);
    u8 prevFlags = state->GetFlags();
    u8 prevCarry = (prevFlags >> 4) & 1;
    u8 newCarry = (prevValue >> 7) & 1;
    u8 newFlags = newCarry ? FLAG_C : 0;

    u8 newValue = (prevValue << 1) | prevCarry;
    if (newValue == 0) {
        newFlags |= FLAG_ZERO;
    }
    return InstructionResult(FlagSet(prevFlags, newFlags), RegisterWrite(reg, prevValue, newValue), 2, 2);
}

InstructionResult Rla(GbCpuState const * state, MemoryState const * memory);

InstructionResult Rlca(GbCpuState const * state, MemoryState const * memory);

template <RegisterName REG>
InstructionResult Rr(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr reg(REG);
    if constexpr (reg.Is16Bit()) {
        u16 location = state->Get16BitRegisterValue(reg);
        u8 prevValue = memory->Read(location);
        u8 prevFlags = state->GetFlags();
        u8 prevCarry = (prevFlags >> 4) & 1;
        u8 newCarry = prevValue & 1;
        u8 newFlags = newCarry ? FLAG_C : 0;

        u8 newValue = (prevValue >> 1) | (prevCarry << 7);
        if (newValue == 0) {
            newFlags |= FLAG_ZERO;
        }
        return InstructionResult(FlagSet(prevFlags, newFlags), MemoryWrite(location, prevValue, newValue), 2, 4);
    } else {
        u8 prevValue = state->Get8BitRegisterValue(reg);
        u8 prevFlags = state->GetFlags();
        u8 prevCarry = (prevFlags >> 4) & 1;
        u8 newCarry = prevValue & 1;
        u8 newFlags = newCarry ? FLAG_C : 0;

        u8 newValue = (prevValue >> 1) | (prevCarry << 7);
        if (newValue == 0) {
            newFlags |= FLAG_ZERO;
        }
        return InstructionResult(FlagSet(prevFlags, newFlags), RegisterWrite(reg, prevValue, newValue), 2, 2);
    }
}

InstructionResult Rra(GbCpuState const * state, MemoryState const * memory);

template <u8 IDX>
InstructionResult Rst(GbCpuState const * state, MemoryState const * memory)
{
    static_assert(IDX < 8);

    Register constexpr pc(RegisterName::PC);
    Register constexpr sp(RegisterName::SP);

    u16 prevSpAddr = state->Get16BitRegisterValue(sp);
    u8 prevHi = memory->Read(prevSpAddr - 1);
    u8 prevLo = memory->Read(prevSpAddr - 2);

    u16 prevPc = state->Get16BitRegisterValue(pc);
    u16 srcValue = prevPc + 1;
    u8 newHi = (srcValue >> 8) & 0xFF;
    u8 newLo = srcValue & 0xFF;

    u16 newPc = IDX * 8;

    return InstructionResult({MemoryWrite(prevSpAddr - 1, prevHi, newHi), MemoryWrite(prevSpAddr - 2, prevLo, newLo)},
                             {RegisterWrite(sp, prevSpAddr, prevSpAddr - 2), RegisterWrite(pc, prevPc, newPc)},
                             0,
                             4);
}

template <RegisterName SRC>
InstructionResult Sbc(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr srcReg(SRC);
    Register constexpr aReg(RegisterName::A);
    u8 prevValue = state->Get8BitRegisterValue(aReg);
    u8 prevFlags = state->GetFlags();
    if constexpr (srcReg.Is16Bit()) {
        static_assert(SRC == RegisterName::HL);
        u16 location = state->Get16BitRegisterValue(srcReg);
        u8 srcValue = memory->Read(location);
        u8 newValue = prevValue - srcValue - ((prevFlags & FLAG_C) ? 1 : 0);

        u8 flags = FLAG_N; // Always set sub flag
        if (newValue == 0) {
            flags |= FLAG_ZERO;
        }
        if (((int)newValue & 0xF) - ((int)srcValue & 0xF) < 0) {
            flags |= FLAG_HC;
        }
        if (((int)newValue) - ((int)srcValue) < 0) {
            flags |= FLAG_C;
        }

        return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(aReg, prevValue, newValue), 1, 2);

    } else {
        u8 srcValue = state->Get8BitRegisterValue(srcReg);
        u8 newValue = prevValue - srcValue - ((prevFlags & FLAG_C) ? 1 : 0);

        u8 flags = FLAG_N; // Always set sub flag
        if (newValue == 0) {
            flags |= FLAG_ZERO;
        }
        if (((int)newValue & 0xF) - ((int)srcValue & 0xF) < 0) {
            flags |= FLAG_HC;
        }
        if (((int)newValue) - ((int)srcValue) < 0) {
            flags |= FLAG_C;
        }

        return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(aReg, prevValue, newValue), 1, 1);
    }
}

InstructionResult SbcD8(GbCpuState const * state, MemoryState const * memory);

template <RegisterName SRC>
InstructionResult Srl(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr srcReg(SRC);
    if constexpr (srcReg.Is16Bit()) {
        u16 location = state->Get16BitRegisterValue(srcReg);
        u8 prevValue = memory->Read(location);
        u8 newValue = (prevValue >> 1) & 0b01111111;

        u8 prevFlags = state->GetFlags();
        u8 newFlags = 0;
        if (newValue == 0) {
            newFlags |= FLAG_ZERO;
        }
        newFlags |= prevValue & 1;

        return InstructionResult(FlagSet(prevFlags, newFlags), MemoryWrite(location, prevValue, newValue), 2, 4);
    } else {
        u8 prevValue = state->Get8BitRegisterValue(srcReg);
        u8 newValue = (prevValue >> 1) & 0b01111111;

        u8 prevFlags = state->GetFlags();
        u8 newFlags = 0;
        if (newValue == 0) {
            newFlags |= FLAG_ZERO;
        }
        newFlags |= prevValue & 1;

        return InstructionResult(FlagSet(prevFlags, newFlags), RegisterWrite(srcReg, prevValue, newValue), 2, 2);
    }
}

template <u8 B>
InstructionResult SetHL(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr reg(RegisterName::HL);

    u16 location = state->Get16BitRegisterValue(reg);
    u8 prevValue = memory->Read(location);
    u8 newValue = prevValue | BIT(B);

    return InstructionResult(MemoryWrite(location, prevValue, newValue), 2, 4);
}

template <RegisterName SRC>
InstructionResult Sla(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr srcReg(SRC);
    static_assert(srcReg.Is8Bit());

    u8 prevValue = state->Get8BitRegisterValue(srcReg);
    u8 newValue = prevValue << 1;

    u8 prevFlags = state->GetFlags();
    u8 newFlags = 0;
    if (newValue == 0) {
        newFlags |= FLAG_ZERO;
    }

    if (prevValue & BIT(7)) {
        newFlags |= FLAG_C;
    }

    return InstructionResult(FlagSet(prevFlags, newFlags), RegisterWrite(srcReg, prevValue, newValue), 2, 2);
}

template <RegisterName SRC>
InstructionResult Sub(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr srcReg(SRC);
    Register constexpr dstReg(RegisterName::A);
    if constexpr (srcReg.Is16Bit()) {
        u16 location = state->Get16BitRegisterValue(srcReg);
        u8 srcValue = memory->Read(location);
        u8 dstValue = state->Get8BitRegisterValue(dstReg);
        u8 newValue = dstValue - srcValue;

        u8 prevFlags = state->GetFlags();
        u8 newFlags = FLAG_N;
        if (newValue == 0) {
            newFlags |= FLAG_ZERO;
        }
        if (((int)dstValue & 0xF) - ((int)srcValue & 0xF) < 0) {
            newFlags |= FLAG_HC;
        }
        if (((int)dstValue) - ((int)srcValue) < 0) {
            newFlags |= FLAG_C;
        }
        return InstructionResult(FlagSet(prevFlags, newFlags), RegisterWrite(dstReg, dstValue, newValue), 1, 2);
    } else {
        u8 dstValue = state->Get8BitRegisterValue(dstReg);
        u8 srcValue = state->Get8BitRegisterValue(srcReg);
        u8 newValue = dstValue - srcValue;

        u8 prevFlags = state->GetFlags();
        u8 flags = FLAG_N; // Always set sub flag
        if (newValue == 0) {
            flags |= FLAG_ZERO;
        }
        if (((int)dstValue & 0xF) - ((int)srcValue & 0xF) < 0) {
            flags |= FLAG_HC;
        }
        if (((int)dstValue) - ((int)srcValue) < 0) {
            flags |= FLAG_C;
        }
        return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(dstReg, dstValue, newValue), 1, 1);
    }
}

InstructionResult SubD8(GbCpuState const * state, MemoryState const * memory);

template <RegisterName REG>
InstructionResult Swap(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr reg(REG);
    if constexpr (reg.Is16Bit()) {
        static_assert(REG == RegisterName::HL);

        u16 hl = state->Get16BitRegisterValue(reg);

        u8 prevValue = memory->Read(hl);
        u8 newValue = prevValue << 4 | prevValue >> 4;

        u8 prevFlags = state->GetFlags();
        u8 newFlags = 0;
        if (newValue == 0) {
            newFlags |= FLAG_ZERO;
        }

        return InstructionResult(FlagSet(prevFlags, newFlags), MemoryWrite(hl, prevValue, newValue), 2, 4);
    } else {
        u8 prevValue = state->Get8BitRegisterValue(reg);
        u8 newValue = prevValue << 4 | prevValue >> 4;

        u8 prevFlags = state->GetFlags();
        u8 newFlags = 0;
        if (newValue == 0) {
            newFlags |= FLAG_ZERO;
        }
        return InstructionResult(FlagSet(prevFlags, newFlags), RegisterWrite(reg, prevValue, newValue), 2, 2);
    }
}

template <bool ENABLED>
InstructionResult ToggleInterrupts(GbCpuState const * state, MemoryState const * memory)
{
    return InstructionResult(InterruptSet(state->GetInterruptMasterEnable(), ENABLED, ENABLED), 1, 1);
}

template <RegisterName SRC>
InstructionResult Xor(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr srcReg(SRC);
    Register constexpr dstReg(RegisterName::A);

    if constexpr (srcReg.Is16Bit()) {
        static_assert(SRC == RegisterName::HL);
        u16 hl = state->Get16BitRegisterValue(srcReg);

        u8 prevValue = state->Get8BitRegisterValue(dstReg);
        u8 xorV = memory->Read(hl);
        u8 newValue = prevValue ^ xorV;

        u8 prevFlags = state->GetFlags();
        u8 flags = 0;
        if (newValue == 0) {
            flags |= FLAG_ZERO;
        }
        return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(dstReg, prevValue, newValue), 1, 2);
    } else {
        u8 prevValue = state->Get8BitRegisterValue(dstReg);
        u8 xorV = state->Get8BitRegisterValue(srcReg);
        u8 newValue = prevValue ^ xorV;

        u8 prevFlags = state->GetFlags();
        u8 flags = 0;
        if (newValue == 0) {
            flags |= FLAG_ZERO;
        }
        return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(dstReg, prevValue, newValue), 1, 1);
    }
}

InstructionResult XorD8(GbCpuState const * state, MemoryState const * memory);
};