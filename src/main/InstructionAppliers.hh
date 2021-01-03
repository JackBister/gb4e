#pragma once

#include "GbCpuState.hh"
#include "Instruction.hh"

namespace gb4e
{
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
        u16 newValue = prevValue + add;

        u8 prevHi = prevValue >> 8;
        u8 newHi = newValue >> 8;

        u8 prevFlags = state->GetFlags();
        u8 flags = prevFlags & FLAG_ZERO; // Preserve zero flag
        if (prevHi < 0b00010000 && newHi >= 0b00010000) {
            flags |= FLAG_HC;
        }
        if (newHi <= prevHi) {
            flags |= FLAG_C;
        }
        return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(dstReg, prevValue, newValue), 1, 2);
    }
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

InstructionResult CpD8(GbCpuState const * state, MemoryState const * memory);

template <RegisterName ADDR>
InstructionResult CpFromMem(GbCpuState const * state, MemoryState const * memory)
{
    static_assert(ADDR == RegisterName::HL, "CpFromMem address register must be HL");
    Register constexpr addrReg(ADDR);
    Register constexpr cpReg(RegisterName::A);

    u16 addr = state->Get16BitRegisterValue(addrReg);
    u8 valueAtAddr = memory->Read(addr);
    u8 valueInReg = state->Get8BitRegisterValue(cpReg);

    u8 prevFlags = state->GetFlags();
    u8 flags = FLAG_N; // Always set N flag
    if (valueAtAddr == valueInReg) {
        flags |= FLAG_ZERO;
    }
    return InstructionResult(FlagSet(prevFlags, flags), 1, 2);
}

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
        return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(reg, prevValue, newValue), 1, 1);
    } else {
        u16 prevValue = state->Get16BitRegisterValue(reg);
        u16 newValue = prevValue - 1;

        return InstructionResult(RegisterWrite(reg, prevValue, newValue), 1, 2);
    }
}

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

InstructionResult JpA16(GbCpuState const * state, MemoryState const * memory);

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

template <RegisterName DST, RegisterName ADDR>
InstructionResult LdFromAddrReg(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr dstReg(DST);
    Register constexpr addrReg(ADDR);
    static_assert(dstReg.Is8Bit());
    static_assert(addrReg.Is16Bit());

    u8 prevValue = state->Get8BitRegisterValue(dstReg);
    u16 addrValue = state->Get16BitRegisterValue(addrReg);
    u8 valueAtAddr = memory->Read(addrValue);
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

template <RegisterName SRC>
InstructionResult Or(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr srcReg(SRC);
    static_assert(srcReg.Is8Bit());
    Register constexpr dstReg(RegisterName::A);

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

InstructionResult Ret(GbCpuState const * state, MemoryState const * memory);

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

template <RegisterName SRC>
InstructionResult Sub(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr srcReg(SRC);
    static_assert(srcReg.Is8Bit());
    Register constexpr dstReg(RegisterName::A);

    u8 prevValue = state->Get8BitRegisterValue(dstReg);
    u8 sub = state->Get8BitRegisterValue(srcReg);
    u8 newValue = prevValue - sub;

    u8 prevFlags = state->GetFlags();
    u8 flags = FLAG_N; // Always set sub flag
    if (newValue == 0) {
        flags |= FLAG_ZERO;
    }
    if (prevValue < 0b00010000 && newValue >= 0b00010000) {
        flags |= FLAG_HC;
    }
    if (newValue >= prevValue && sub != 0) {
        flags |= FLAG_C;
    }
    return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(dstReg, prevValue, newValue), 1, 1);
}

template <RegisterName SRC>
InstructionResult Xor(GbCpuState const * state, MemoryState const * memory)
{
    Register constexpr srcReg(SRC);
    static_assert(srcReg.Is8Bit());
    Register constexpr dstReg(RegisterName::A);

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
};