#include "InstructionAppliers.hh"

#include "GbCpuState.hh"

namespace gb4e
{
InstructionApplier Add(RegisterName dstRegName, RegisterName srcRegName)
{
    auto dstReg = GetRegister(dstRegName);
    auto srcReg = GetRegister(srcRegName);

    if (dstReg->Is8Bit()) {
        assert(dstReg->GetRegisterName() == RegisterName::A);
        return [dstReg, srcReg](GbCpuState const * state) {
            u8 prevValue = state->Get8BitRegisterValue(dstReg);
            u8 add = state->Get8BitRegisterValue(srcReg);
            u8 newValue = prevValue + add;

            u8 prevFlags = state->GetFlags();
            u8 flags = 0;
            if (newValue == 0) {
                flags |= 0b10000000; // zero flag
            }
            if (prevValue < 0b00010000 && newValue >= 0b00010000) {
                flags |= 0b00100000; // half carry flag
            }
            if (newValue <= prevValue && add != 0) {
                flags |= 0b00010000; // carry flag
            }
            return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(dstReg, prevValue, newValue), 1, 1);
        };
    } else {
        assert(dstReg->GetRegisterName() == RegisterName::HL);
        assert(srcReg->Is16Bit());
        return [dstReg, srcReg](GbCpuState const * state) {
            u16 prevValue = state->Get16BitRegisterValue(dstReg);
            u16 add = state->Get16BitRegisterValue(srcReg);
            u16 newValue = prevValue + add;

            u8 prevHi = prevValue >> 8;
            u8 newHi = newValue >> 8;

            u8 prevFlags = state->GetFlags();
            u8 flags = prevFlags & 0b10000000;
            if (prevHi < 0b00010000 && newHi >= 0b00010000) {
                flags |= 0b00100000; // half carry flag
            }
            if (newHi <= prevHi) {
                flags |= 0b00010000; // carry flag
            }
            return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(dstReg, prevValue, newValue), 1, 2);
        };
    }
}

InstructionApplier AddFromAddrReg(RegisterName dstRegName, RegisterName addrRegName)
{
    assert(dstRegName == RegisterName::A);
    assert(addrRegName == RegisterName::HL);
    return [dstRegName, addrRegName](GbCpuState const * state) {
        auto dstReg = GetRegister(dstRegName);
        auto addrReg = GetRegister(addrRegName);

        u8 prevValue = state->Get8BitRegisterValue(dstReg);
        u16 addr = state->Get16BitRegisterValue(addrReg);
        u8 add = state->Get8BitMemoryValue(addr);

        u8 newValue = prevValue + add;

        u8 prevFlags = state->GetFlags();
        u8 flags = 0;
        if (newValue == 0) {
            flags |= 0b10000000; // zero flag
        }
        if (prevValue < 0b00010000 && newValue >= 0b00010000) {
            flags |= 0b00100000; // half carry flag
        }
        if (newValue <= prevValue && add != 0) {
            flags |= 0b00010000; // carry flag
        }
        return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(dstReg, prevValue, newValue), 1, 2);
    };
}

InstructionApplier And(RegisterName srcRegName)
{
    auto srcReg = GetRegister(srcRegName);
    assert(srcReg->Is8Bit());
    return [srcReg](GbCpuState const * state) {
        auto dstReg = GetRegister(RegisterName::A);
        u8 prevValue = state->Get8BitRegisterValue(dstReg);
        u8 andV = state->Get8BitRegisterValue(srcReg);
        u8 newValue = prevValue & andV;

        u8 prevFlags = state->GetFlags();
        u8 flags = 0b00100000;
        if (newValue == 0) {
            flags |= 0b10000000;
        }
        return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(dstReg, prevValue, newValue), 1, 1);
    };
}

InstructionApplier Bit(u8 bit, RegisterName regName)
{
    auto reg = GetRegister(regName);
    assert(reg->Is8Bit());
    assert(bit < 8);
    return [reg, bit](GbCpuState const * state) {
        u8 regValue = state->Get8BitRegisterValue(reg);
        u8 value = (regValue >> bit) & 1;

        u8 prevFlags = state->GetFlags();
        u8 flags = prevFlags & 0b00010000;
        flags |= 0b00100000;
        if (!value) {
            flags |= 0b10000000;
        }
        return InstructionResult(FlagSet(prevFlags, flags), 2, 2);
    };
}

InstructionApplier CallA16()
{
    return [](GbCpuState const * state) {
        auto pc = GetRegister(RegisterName::PC);
        auto sp = GetRegister(RegisterName::SP);

        u16 prevSpAddr = state->Get16BitRegisterValue(sp);
        u8 prevHi = state->Get8BitMemoryValue(prevSpAddr - 1);
        u8 prevLo = state->Get8BitMemoryValue(prevSpAddr - 2);

        u16 prevPc = state->Get16BitRegisterValue(pc);
        u16 srcValue = prevPc + 3;
        u8 newHi = (srcValue >> 8) & 0xFF;
        u8 newLo = srcValue & 0xFF;

        u16 newPc = state->Get16BitMemoryValue(prevPc + 1);

        return InstructionResult(
            {MemoryWrite(prevSpAddr - 1, prevHi, newHi), MemoryWrite(prevSpAddr - 2, prevLo, newLo)},
            {RegisterWrite(sp, prevSpAddr, prevSpAddr - 2), RegisterWrite(pc, prevPc, newPc)},
            0, // TODO: Not sure I like this, this prevents GbCpuState::ApplyInstructionResult from adding even more to
               // PC, since CALL should already put you at the final address. An alternative would be if all
               // instructions instead returned a RegisterWrite to PC, but that could get pretty messy too.
            6);
    };
}

InstructionApplier CpD8()
{
    return [](GbCpuState const * state) {
        auto pc = GetRegister(RegisterName::PC);
        auto cpReg = GetRegister(RegisterName::A);

        u16 pcValue = state->Get16BitRegisterValue(pc);
        assert(pcValue < 0xFFFF);

        u8 valueAtAddr = state->Get8BitMemoryValue(pcValue + 1);
        u8 valueInReg = state->Get8BitRegisterValue(cpReg);

        u8 prevFlags = state->GetFlags();
        u8 flags = 0b01000000;
        if (valueAtAddr == valueInReg) {
            flags |= 0b10000000;
        }
        return InstructionResult(FlagSet(prevFlags, flags), 2, 2);
    };
}

InstructionApplier CpFromMem(RegisterName addrRegName)
{
    assert(addrRegName == RegisterName::HL);
    return [addrRegName](GbCpuState const * state) {
        auto addrReg = GetRegister(addrRegName);
        auto cpReg = GetRegister(RegisterName::A);

        u16 addr = state->Get16BitRegisterValue(addrReg);
        u8 valueAtAddr = state->Get8BitMemoryValue(addr);
        u8 valueInReg = state->Get8BitRegisterValue(cpReg);

        u8 prevFlags = state->GetFlags();
        u8 flags = 0b01000000;
        if (valueAtAddr == valueInReg) {
            flags |= 0b10000000;
        }
        return InstructionResult(FlagSet(prevFlags, flags), 1, 2);
    };
}

InstructionApplier Dec(RegisterName regName)
{
    auto reg = GetRegister(regName);
    if (reg->Is8Bit()) {
        return [reg](GbCpuState const * state) {
            u8 prevValue = state->Get8BitRegisterValue(reg);
            u8 newValue = prevValue - 1;

            u8 prevFlags = state->GetFlags();
            u8 flags = 0;
            flags |= (prevFlags & 0b00010000); // Keep carry flag
            flags |= 0b01000000;               // Always set sub flag
            if (newValue == 0) {
                flags |= 0b10000000; // zero flag
            }
            return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(reg, prevValue, newValue), 1, 1);
        };
    } else {
        return [reg](GbCpuState const * state) {
            u16 prevValue = state->Get16BitRegisterValue(reg);
            u16 newValue = prevValue - 1;

            return InstructionResult(RegisterWrite(reg, prevValue, newValue), 1, 2);
        };
    }
}

InstructionApplier Inc(RegisterName regName)
{
    auto reg = GetRegister(regName);
    if (reg->Is8Bit()) {
        return [reg](GbCpuState const * state) {
            u8 prevValue = state->Get8BitRegisterValue(reg);
            u8 newValue = prevValue + 1;

            u8 prevFlags = state->GetFlags();
            u8 flags = 0;
            flags |= (prevFlags & 0b00010000); // Keep carry flag
            if (newValue == 0) {
                flags |= 0b10000000; // zero flag
            }
            if (prevValue == 0b00001111) {
                flags |= 0b00100000; // half carry flag
            }
            return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(reg, prevValue, newValue), 1, 1);
        };
    } else {
        return [reg](GbCpuState const * state) {
            u16 prevValue = state->Get16BitRegisterValue(reg);
            u16 newValue = prevValue + 1;

            return InstructionResult(RegisterWrite(reg, prevValue, newValue), 1, 2);
        };
    }
}
InstructionApplier JrFlagS8(u8 flagmask)
{
    return [flagmask](GbCpuState const * state) {
        auto pc = GetRegister(RegisterName::PC);

        u8 flags = state->GetFlags();
        if (flags & flagmask) {
            u16 pcPrevValue = state->Get16BitRegisterValue(pc);
            assert(pcPrevValue < 0xFFFF);
            s8 imm = state->Get8BitMemoryValue(pcPrevValue + 1);
            return InstructionResult(RegisterWrite(pc, pcPrevValue, pcPrevValue + imm), 2, 3);
        } else {
            return InstructionResult(2, 2);
        }
    };
}

InstructionApplier JrNFlagS8(u8 flagmask)
{
    return [flagmask](GbCpuState const * state) {
        auto pc = GetRegister(RegisterName::PC);

        u8 flags = state->GetFlags();
        if (!(flags & flagmask)) {
            u16 pcPrevValue = state->Get16BitRegisterValue(pc);
            assert(pcPrevValue < 0xFFFF);
            s8 imm = state->Get8BitMemoryValue(pcPrevValue + 1);
            return InstructionResult(RegisterWrite(pc, pcPrevValue, pcPrevValue + imm), 2, 3);
        } else {
            return InstructionResult(2, 2);
        }
    };
}

InstructionApplier JrS8()
{
    return [](GbCpuState const * state) {
        auto pc = GetRegister(RegisterName::PC);

        u16 pcPrevValue = state->Get16BitRegisterValue(pc);
        assert(pcPrevValue < 0xFFFF);
        s8 imm = state->Get8BitMemoryValue(pcPrevValue + 1);
        return InstructionResult(RegisterWrite(pc, pcPrevValue, pcPrevValue + imm), 2, 3);
    };
}

InstructionApplier Ld(RegisterName dstRegName, RegisterName srcRegName)
{
    return [dstRegName, srcRegName](GbCpuState const * state) {
        auto dstReg = GetRegister(dstRegName);
        auto srcReg = GetRegister(srcRegName);
        assert(dstReg->Is8Bit() && srcReg->Is8Bit());

        u8 dstPrevValue = state->Get8BitRegisterValue(dstReg);
        u8 srcValue = state->Get8BitRegisterValue(srcReg);
        return InstructionResult(RegisterWrite(dstReg, dstPrevValue, srcValue), 1, 1);
    };
}

InstructionApplier LdA8()
{
    return [](GbCpuState const * state) {
        auto pc = GetRegister(RegisterName::PC);
        auto srcReg = GetRegister(RegisterName::A);

        u16 pcValue = state->Get16BitRegisterValue(pc);
        assert(pcValue < 0xFFFF);
        u8 addrImm = state->Get8BitMemoryValue(pcValue + 1);
        u16 addr = 0xFF00 + addrImm;

        u8 prevValue = state->Get8BitMemoryValue(addr);
        u8 newValue = state->Get8BitRegisterValue(srcReg);

        return InstructionResult(MemoryWrite(addr, prevValue, newValue), 2, 3);
    };
}

InstructionApplier LdA16(RegisterName srcRegName)
{
    assert(srcRegName == RegisterName::A || srcRegName == RegisterName::SP);
    auto srcReg = GetRegister(srcRegName);
    if (srcReg->Is8Bit()) {
        return [srcReg](GbCpuState const * state) {
            auto pc = GetRegister(RegisterName::PC);

            u16 pcValue = state->Get16BitRegisterValue(pc);
            assert(pcValue < 0xFFFE);
            u16 addr = state->Get16BitMemoryValue(pcValue + 1);

            u8 prevValue = state->Get8BitMemoryValue(addr);
            u8 newValue = state->Get8BitRegisterValue(srcReg);

            return InstructionResult(MemoryWrite(addr, prevValue, newValue), 3, 4);
        };
    } else {
        return [srcReg](GbCpuState const * state) {
            auto pc = GetRegister(RegisterName::PC);

            u16 pcValue = state->Get16BitRegisterValue(pc);
            assert(pcValue < 0xFFFE);
            u16 addr = state->Get16BitMemoryValue(pcValue + 1);

            u8 prevLo = state->Get8BitMemoryValue(addr);
            u8 prevHi = state->Get8BitMemoryValue(addr + 1);

            u16 srcValue = state->Get16BitRegisterValue(srcReg);
            u8 newLo = srcValue & 0xFF;
            u8 newHi = (srcValue >> 8) & 0xFF;
            return InstructionResult({MemoryWrite(addr, prevLo, newLo), MemoryWrite(addr + 1, prevHi, newHi)}, 3, 5);
        };
    }
}

InstructionApplier LdD8(RegisterName dstRegName)
{
    return [dstRegName](GbCpuState const * state) {
        auto dstReg = GetRegister(dstRegName);
        assert(dstReg->Is8Bit());
        auto pc = GetRegister(RegisterName::PC);

        u8 dstPrevValue = state->Get8BitRegisterValue(dstReg);
        u16 pcValue = state->Get16BitRegisterValue(pc);
        assert(pcValue < 0xFFFF);
        u8 imm = state->Get8BitMemoryValue(pcValue + 1);
        return InstructionResult(RegisterWrite(dstReg, dstPrevValue, imm), 2, 2);
    };
}

InstructionApplier LdD16(RegisterName dstRegName)
{
    return [dstRegName](GbCpuState const * state) {
        auto dstReg = GetRegister(dstRegName);
        assert(dstReg->Is16Bit());
        auto pc = GetRegister(RegisterName::PC);

        u16 dstPrevValue = state->Get16BitRegisterValue(dstReg);
        u16 pcValue = state->Get16BitRegisterValue(pc);
        assert(pcValue < 0xFFFE);
        u16 imm = state->Get16BitMemoryValue(pcValue + 1);
        return InstructionResult(RegisterWrite(dstReg, dstPrevValue, imm), 3, 3);
    };
}

InstructionApplier LdFromAddrReg(RegisterName dstRegName, RegisterName addrRegName)
{
    return [dstRegName, addrRegName](GbCpuState const * state) {
        auto dstReg = GetRegister(dstRegName);
        assert(dstReg->Is8Bit());
        auto addrReg = GetRegister(addrRegName);
        assert(addrReg->Is16Bit());

        u8 prevValue = state->Get8BitRegisterValue(dstReg);
        u16 addrValue = state->Get16BitRegisterValue(addrReg);
        u8 valueAtAddr = state->Get8BitMemoryValue(addrValue);
        return InstructionResult(RegisterWrite(dstReg, prevValue, valueAtAddr), 1, 2);
    };
}

InstructionApplier LdFromA8(RegisterName dstRegName)
{
    auto dstReg = GetRegister(dstRegName);
    assert(dstReg->Is8Bit());
    return [dstReg](GbCpuState const * state) {
        auto pc = GetRegister(RegisterName::PC);

        u16 pcValue = state->Get16BitRegisterValue(pc);
        assert(pcValue < 0xFFFF);
        u8 addrImm = state->Get8BitMemoryValue(pcValue + 1);
        u16 addr = 0xFF00 + addrImm;

        u8 prevValue = state->Get8BitRegisterValue(dstReg);
        u8 newValue = state->Get8BitMemoryValue(addr);
        return InstructionResult(RegisterWrite(dstReg, prevValue, newValue), 2, 3);
    };
}

InstructionApplier LdFromA16(RegisterName dstRegName)
{
    auto dstReg = GetRegister(dstRegName);
    assert(dstReg->Is8Bit());
    return [dstReg](GbCpuState const * state) {
        auto pc = GetRegister(RegisterName::PC);

        u16 pcValue = state->Get16BitRegisterValue(pc);
        assert(pcValue < 0xFFFE);
        u16 addr = state->Get16BitMemoryValue(pcValue + 1);

        u8 prevValue = state->Get8BitRegisterValue(dstReg);
        u8 newValue = state->Get8BitMemoryValue(addr);
        return InstructionResult(RegisterWrite(dstReg, prevValue, newValue), 3, 4);
    };
}

InstructionApplier LdHlIncDecA(s8 modifier)
{
    assert(modifier == 1 || modifier == -1);
    return [modifier](GbCpuState const * state) {
        auto hl = GetRegister(RegisterName::HL);
        auto a = GetRegister(RegisterName::A);

        u16 hlPrevValue = state->Get16BitRegisterValue(hl);
        u8 memPrevValue = state->Get8BitMemoryValue(hlPrevValue);
        u8 aValue = state->Get8BitRegisterValue(a);

        return InstructionResult(MemoryWrite(hlPrevValue, memPrevValue, aValue),
                                 RegisterWrite(hl, hlPrevValue, hlPrevValue + modifier),
                                 1,
                                 2);
    };
}

InstructionApplier LdMemViaReg(RegisterName addrRegName, RegisterName srcRegName)
{
    auto addrReg = GetRegister(addrRegName);
    auto srcReg = GetRegister(srcRegName);
    assert(srcReg->Is8Bit());
    if (addrReg->Is8Bit()) {
        assert(addrReg->GetRegisterName() == RegisterName::C);
        return [addrReg, srcReg](GbCpuState const * state) {
            u8 addrOffset = state->Get8BitRegisterValue(addrReg);
            u16 addr = 0xFF00 + addrOffset;
            u8 srcValue = state->Get8BitRegisterValue(srcReg);
            u8 prevValue = state->Get8BitMemoryValue(addr);
            return InstructionResult(MemoryWrite(addr, prevValue, srcValue), 1, 2);
        };
    } else {
        assert(addrReg->GetRegisterName() == RegisterName::BC || addrReg->GetRegisterName() == RegisterName::DE ||
               addrReg->GetRegisterName() == RegisterName::HL);
        return [addrReg, srcReg](GbCpuState const * state) {
            u16 addr = state->Get16BitRegisterValue(addrReg);
            u8 srcValue = state->Get8BitRegisterValue(srcReg);
            u8 prevValue = state->Get8BitMemoryValue(addr);
            return InstructionResult(MemoryWrite(addr, prevValue, srcValue), 1, 2);
        };
    }
}

InstructionApplier Or(RegisterName srcRegName)
{
    auto srcReg = GetRegister(srcRegName);
    assert(srcReg->Is8Bit());
    return [srcReg](GbCpuState const * state) {
        auto dstReg = GetRegister(RegisterName::A);

        u8 prevValue = state->Get8BitRegisterValue(dstReg);
        u8 orV = state->Get8BitRegisterValue(srcReg);
        u8 newValue = prevValue | orV;

        u8 prevFlags = state->GetFlags();
        u8 flags = 0;
        if (newValue == 0) {
            flags |= 0b10000000;
        }
        return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(dstReg, prevValue, newValue), 1, 1);
    };
}

InstructionApplier Pop(RegisterName dstRegName)
{
    auto dstReg = GetRegister(dstRegName);
    assert(dstReg->Is16Bit());
    return [dstReg](GbCpuState const * state) {
        auto sp = GetRegister(RegisterName::SP);

        u16 prevSpAddr = state->Get16BitRegisterValue(sp);
        u16 prevValue = state->Get16BitRegisterValue(dstReg);

        u8 popLo = state->Get8BitMemoryValue(prevSpAddr);
        u8 popHi = state->Get8BitMemoryValue(prevSpAddr + 1);

        u16 newValue = ((u16)popLo) | (((u16)popHi) << 8);
        return InstructionResult(
            {RegisterWrite(sp, prevSpAddr, prevSpAddr + 2), RegisterWrite(dstReg, prevValue, newValue)}, 1, 3);
    };
}

InstructionApplier Push(RegisterName srcRegName)
{
    auto srcReg = GetRegister(srcRegName);
    assert(srcReg->Is16Bit());
    return [srcReg](GbCpuState const * state) {
        auto sp = GetRegister(RegisterName::SP);

        u16 prevSpAddr = state->Get16BitRegisterValue(sp);
        u8 prevHi = state->Get8BitMemoryValue(prevSpAddr - 1);
        u8 prevLo = state->Get8BitMemoryValue(prevSpAddr - 2);

        u16 srcValue = state->Get16BitRegisterValue(srcReg);
        u8 newHi = (srcValue >> 8) & 0xFF;
        u8 newLo = srcValue & 0xFF;

        return InstructionResult(
            {MemoryWrite(prevSpAddr - 1, prevHi, newHi), MemoryWrite(prevSpAddr - 2, prevLo, newLo)},
            RegisterWrite(sp, prevSpAddr, prevSpAddr - 2),
            1,
            4);
    };
}

InstructionApplier Ret()
{
    return [](GbCpuState const * state) {
        auto pc = GetRegister(RegisterName::PC);
        auto sp = GetRegister(RegisterName::SP);

        u16 prevSpAddr = state->Get16BitRegisterValue(sp);
        assert(prevSpAddr < 0xFFFF);
        u8 pcLo = state->Get8BitMemoryValue(prevSpAddr);
        u8 pcHi = state->Get8BitMemoryValue(prevSpAddr + 1);

        u16 prevPcValue = state->Get16BitRegisterValue(pc);
        u16 newPcValue = ((u16)pcLo) | (((u16)pcHi) << 8);

        return InstructionResult(
            {RegisterWrite(sp, prevSpAddr, prevSpAddr + 2), RegisterWrite(pc, prevPcValue, newPcValue)}, 1, 4);
    };
}

InstructionApplier RetNFlag(u8 flagmask)
{
    return [flagmask](GbCpuState const * state) {
        u8 flags = state->GetFlags();
        if (!(flags & flagmask)) {
            auto pc = GetRegister(RegisterName::PC);
            auto sp = GetRegister(RegisterName::SP);

            u16 prevSpAddr = state->Get16BitRegisterValue(sp);
            assert(prevSpAddr < 0xFFFF);
            u8 pcLo = state->Get8BitMemoryValue(prevSpAddr);
            u8 pcHi = state->Get8BitMemoryValue(prevSpAddr + 1);

            u16 prevPcValue = state->Get16BitRegisterValue(pc);
            u16 newPcValue = ((u16)pcLo) | (((u16)pcHi) << 8);

            return InstructionResult(
                {RegisterWrite(sp, prevSpAddr, prevSpAddr + 2), RegisterWrite(pc, prevPcValue, newPcValue)}, 1, 5);
        } else {
            return InstructionResult(1, 2);
        }
    };
}

InstructionApplier Rl(RegisterName regName)
{
    auto reg = GetRegister(regName);
    assert(reg->Is8Bit());
    return [reg](GbCpuState const * state) {
        u8 prevValue = state->Get8BitRegisterValue(reg);
        u8 prevFlags = state->GetFlags();
        u8 prevCarry = (prevFlags >> 4) & 0b00000001;
        u8 newCarry = (prevValue >> 7) & 0b00000001;
        u8 newFlags = ((newCarry) << 4) & 0b00010000;

        u8 newValue = (prevValue << 1) | prevCarry;
        if (newValue == 0) {
            newFlags |= 0b10000000;
        }
        return InstructionResult(FlagSet(prevFlags, newFlags), RegisterWrite(reg, prevValue, newValue), 2, 2);
    };
}

InstructionApplier Rla()
{
    return [](GbCpuState const * state) {
        auto reg = GetRegister(RegisterName::A);

        u8 prevValue = state->Get8BitRegisterValue(reg);
        u8 prevFlags = state->GetFlags();
        u8 prevCarry = (prevFlags >> 4) & 0b00000001;
        u8 newCarry = (prevValue >> 7) & 0b00000001;
        u8 newFlags = ((newCarry) << 4) & 0b00010000;

        u8 newValue = (prevValue << 1) | prevCarry;
        return InstructionResult(FlagSet(prevFlags, newFlags), RegisterWrite(reg, prevValue, newValue), 1, 1);
    };
}

InstructionApplier Sub(RegisterName srcRegName)
{
    auto srcReg = GetRegister(srcRegName);
    assert(srcReg->Is8Bit());
    return [srcReg](GbCpuState const * state) {
        auto dstReg = GetRegister(RegisterName::A);

        u8 prevValue = state->Get8BitRegisterValue(dstReg);
        u8 sub = state->Get8BitRegisterValue(srcReg);
        u8 newValue = prevValue - sub;

        u8 prevFlags = state->GetFlags();
        u8 flags = 0b01000000;
        if (newValue == 0) {
            flags |= 0b10000000; // zero flag
        }
        if (prevValue < 0b00010000 && newValue >= 0b00010000) {
            flags |= 0b00100000; // half carry flag
        }
        if (newValue >= prevValue && sub != 0) {
            flags |= 0b00010000; // carry flag
        }
        return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(dstReg, prevValue, newValue), 1, 1);
    };
}

InstructionApplier Xor(RegisterName srcRegName)
{
    auto srcReg = GetRegister(srcRegName);
    assert(srcReg->Is8Bit());
    return [srcReg](GbCpuState const * state) {
        auto dstReg = GetRegister(RegisterName::A);

        u8 prevValue = state->Get8BitRegisterValue(dstReg);
        u8 xorV = state->Get8BitRegisterValue(srcReg);
        u8 newValue = prevValue ^ xorV;

        u8 prevFlags = state->GetFlags();
        u8 flags = 0;
        if (newValue == 0) {
            flags |= 0b10000000;
        }
        return InstructionResult(FlagSet(prevFlags, flags), RegisterWrite(dstReg, prevValue, newValue), 1, 1);
    };
}
};
