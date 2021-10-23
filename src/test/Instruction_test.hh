#pragma once

#include "greatest.h"

#include "GbCpuState.hh"
#include "InstructionAppliers.hh"
#include "Register.hh"

TEST Instr_Ld_B_A()
{
    using namespace gb4e;
    auto applier = Ld<RegisterName::B, RegisterName::A>;
    GbCpuState state;
    MemoryStateFake memory;
    auto regA = GetRegister(RegisterName::A);
    auto regB = GetRegister(RegisterName::B);
    state.Set8BitRegisterValue(regA, 123);
    state.Set8BitRegisterValue(regB, 100);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::B, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(100, regWrite.GetBytePreviousValue());
    ASSERT_EQ(123, regWrite.GetByteValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(1, result.GetConsumedCycles());
    PASS();
}

TEST Instr_Ld_SP_D16()
{
    using namespace gb4e;
    auto applier = LdD16<RegisterName::SP>;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set16BitRegisterValue(GetRegister(RegisterName::PC), 0);
    state.Set16BitRegisterValue(GetRegister(RegisterName::SP), 0xBEEF);
    memory.Write(0, 0x31);
    memory.Write(1, 0xFE);
    memory.Write(2, 0xFF);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::SP, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0xBEEF, regWrite.GetWordPreviousValue());
    ASSERT_EQ(0xFFFE, regWrite.GetWordValue());
    ASSERT_EQ(3, result.GetConsumedBytes());
    ASSERT_EQ(3, result.GetConsumedCycles());
    PASS();
}

TEST Instr_Inc_BC()
{
    using namespace gb4e;
    auto applier = Inc<RegisterName::BC>;
    GbCpuState state;
    MemoryStateFake memory;
    auto regBC = GetRegister(RegisterName::BC);
    state.Set16BitRegisterValue(regBC, 100);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::BC, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(100, regWrite.GetWordPreviousValue());
    ASSERT_EQ(101, regWrite.GetWordValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());

    PASS();
}

TEST Instr_Inc_BC_Wraparound()
{
    using namespace gb4e;
    auto applier = Inc<RegisterName::BC>;
    GbCpuState state;
    MemoryStateFake memory;
    auto regBC = GetRegister(RegisterName::BC);
    state.Set16BitRegisterValue(regBC, 0xFFFF);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::BC, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0xFFFF, regWrite.GetWordPreviousValue());
    ASSERT_EQ(0, regWrite.GetWordValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());

    PASS();
}

TEST Instr_Inc_A()
{
    using namespace gb4e;
    auto applier = Inc<RegisterName::A>;
    GbCpuState state;
    MemoryStateFake memory;
    auto regA = GetRegister(RegisterName::A);
    state.Set8BitRegisterValue(regA, 5);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(5, regWrite.GetBytePreviousValue());
    ASSERT_EQ(6, regWrite.GetByteValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(1, result.GetConsumedCycles());

    PASS();
}

TEST Instr_Inc_A_Wraparound()
{
    using namespace gb4e;
    auto applier = Inc<RegisterName::A>;
    GbCpuState state;
    MemoryStateFake memory;
    auto regA = GetRegister(RegisterName::A);
    state.Set8BitRegisterValue(regA, 0xFF);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0xFF, regWrite.GetBytePreviousValue());
    ASSERT_EQ(0, regWrite.GetByteValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(1, result.GetConsumedCycles());

    PASS();
}

TEST Instr_Inc_A_HalfCarry()
{
    using namespace gb4e;
    auto applier = Inc<RegisterName::A>;
    GbCpuState state;
    MemoryStateFake memory;
    auto regA = GetRegister(RegisterName::A);
    state.Set8BitRegisterValue(regA, 0b00001111);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0b00001111, regWrite.GetBytePreviousValue());
    ASSERT_EQ(0b00010000, regWrite.GetByteValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(1, result.GetConsumedCycles());

    PASS();
}

TEST Instr_Inc_A_PreservesCarry()
{
    using namespace gb4e;
    auto applier = Inc<RegisterName::A>;
    GbCpuState state;
    MemoryStateFake memory;
    auto regA = GetRegister(RegisterName::A);
    state.Set8BitRegisterValue(regA, 5);
    state.SetFlags(0b00010000);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT_EQ(0b00010000, result.GetFlagSet().value().GetValue());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(5, regWrite.GetBytePreviousValue());
    ASSERT_EQ(6, regWrite.GetByteValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(1, result.GetConsumedCycles());

    PASS();
}

TEST Instr_Dec_BC()
{
    using namespace gb4e;
    auto applier = Dec<RegisterName::BC>;
    GbCpuState state;
    MemoryStateFake memory;
    auto regBC = GetRegister(RegisterName::BC);
    state.Set16BitRegisterValue(regBC, 100);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::BC, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(100, regWrite.GetWordPreviousValue());
    ASSERT_EQ(99, regWrite.GetWordValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());

    PASS();
}

TEST Instr_Dec_BC_Wraparound()
{
    using namespace gb4e;
    auto applier = Dec<RegisterName::BC>;
    GbCpuState state;
    MemoryStateFake memory;
    auto regBC = GetRegister(RegisterName::BC);
    state.Set16BitRegisterValue(regBC, 0);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::BC, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0, regWrite.GetWordPreviousValue());
    ASSERT_EQ(0xFFFF, regWrite.GetWordValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());

    PASS();
}

TEST Instr_Dec_A()
{
    using namespace gb4e;
    auto applier = Dec<RegisterName::A>;
    GbCpuState state;
    MemoryStateFake memory;
    auto regA = GetRegister(RegisterName::A);
    state.Set8BitRegisterValue(regA, 5);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(5, regWrite.GetBytePreviousValue());
    ASSERT_EQ(4, regWrite.GetByteValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(1, result.GetConsumedCycles());

    PASS();
}

TEST Instr_Dec_A_Zero()
{
    using namespace gb4e;
    auto applier = Dec<RegisterName::A>;
    GbCpuState state;
    MemoryStateFake memory;
    auto regA = GetRegister(RegisterName::A);
    state.Set8BitRegisterValue(regA, 1);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(1, regWrite.GetBytePreviousValue());
    ASSERT_EQ(0, regWrite.GetByteValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(1, result.GetConsumedCycles());

    PASS();
}

TEST Instr_Dec_A_Wraparound()
{
    using namespace gb4e;
    auto applier = Dec<RegisterName::A>;
    GbCpuState state;
    MemoryStateFake memory;
    auto regA = GetRegister(RegisterName::A);
    state.Set8BitRegisterValue(regA, 0);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0, regWrite.GetBytePreviousValue());
    ASSERT_EQ(0xFF, regWrite.GetByteValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(1, result.GetConsumedCycles());

    PASS();
}

TEST Instr_Dec_A_PreservesCarry()
{
    using namespace gb4e;
    auto applier = Dec<RegisterName::A>;
    GbCpuState state;
    MemoryStateFake memory;
    auto regA = GetRegister(RegisterName::A);
    state.Set8BitRegisterValue(regA, 5);
    state.SetFlags(0b00010000);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT_EQ(0b01010000, result.GetFlagSet().value().GetValue());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(5, regWrite.GetBytePreviousValue());
    ASSERT_EQ(4, regWrite.GetByteValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(1, result.GetConsumedCycles());

    PASS();
}

TEST Instr_LdD8_A()
{

    using namespace gb4e;
    auto applier = LdD8<RegisterName::A>;
    GbCpuState state;
    MemoryStateFake memory;
    auto regA = GetRegister(RegisterName::A);
    state.Set8BitRegisterValue(regA, 100);
    state.Set16BitRegisterValue(GetRegister(RegisterName::PC), 0);
    memory.Write(0, 0x3E);
    memory.Write(1, 123);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(100, regWrite.GetBytePreviousValue());
    ASSERT_EQ(123, regWrite.GetByteValue());
    ASSERT_EQ(2, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());

    PASS();
}

TEST Instr_LdD16_BC()
{
    using namespace gb4e;
    auto applier = LdD16<RegisterName::BC>;
    GbCpuState state;
    MemoryStateFake memory;
    auto regBC = GetRegister(RegisterName::BC);
    state.Set16BitRegisterValue(regBC, 100);
    state.Set16BitRegisterValue(GetRegister(RegisterName::PC), 0);
    memory.Write(0, 0x01);
    memory.Write(1, 0xEF);
    memory.Write(2, 0xBE);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::BC, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(100, regWrite.GetWordPreviousValue());
    ASSERT_EQ(0xBEEF, regWrite.GetWordValue());
    ASSERT_EQ(3, result.GetConsumedBytes());
    ASSERT_EQ(3, result.GetConsumedCycles());

    PASS();
}

TEST Instr_Rla_NoCarry()
{
    using namespace gb4e;
    auto applier = Rla;
    GbCpuState state;
    MemoryStateFake memory;
    auto regA = GetRegister(RegisterName::A);
    state.Set8BitRegisterValue(regA, 0b00000001);
    state.SetFlags(0b11100000);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT_EQ(0, result.GetFlagSet().value().GetValue());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0b00000001, regWrite.GetBytePreviousValue());
    ASSERT_EQ(0b00000010, regWrite.GetByteValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(1, result.GetConsumedCycles());

    PASS();
}

TEST Instr_Rla_Carry()
{
    using namespace gb4e;
    auto applier = Rla;
    GbCpuState state;
    MemoryStateFake memory;
    auto regA = GetRegister(RegisterName::A);
    state.Set8BitRegisterValue(regA, 0b10000000);
    state.SetFlags(0b11110000);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT_EQ(0b00010000, result.GetFlagSet().value().GetValue());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0b10000000, regWrite.GetBytePreviousValue());
    ASSERT_EQ(0b00000001, regWrite.GetByteValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(1, result.GetConsumedCycles());

    PASS();
}

TEST Instr_JrS8()
{
    using namespace gb4e;
    auto applier = JrS8;
    GbCpuState state;
    MemoryStateFake memory;
    auto regPC = GetRegister(RegisterName::PC);
    state.Set16BitRegisterValue(regPC, 0);
    memory.Write(0x0, 0x18);
    memory.Write(0x1, 100);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::PC, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0, regWrite.GetWordPreviousValue());
    ASSERT_EQ(100, regWrite.GetWordValue());
    ASSERT_EQ(2, result.GetConsumedBytes());
    ASSERT_EQ(3, result.GetConsumedCycles());

    PASS();
}

TEST Instr_LdFromAddrReg_A_DE()
{
    using namespace gb4e;
    auto applier = LdFromAddrReg<RegisterName::A, RegisterName::DE>;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set8BitRegisterValue(GetRegister(RegisterName::A), 100);
    state.Set16BitRegisterValue(GetRegister(RegisterName::DE), 0x1);
    memory.Write(0x1, 123);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(100, regWrite.GetBytePreviousValue());
    ASSERT_EQ(123, regWrite.GetByteValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());

    PASS();
}

TEST Instr_LdFromAddrReg_WithModifier()
{
    using namespace gb4e;
    auto applier = LdFromAddrReg<RegisterName::A, RegisterName::HL, -1>;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set8BitRegisterValue(GetRegister(RegisterName::A), 100);
    state.Set16BitRegisterValue(GetRegister(RegisterName::HL), 100);
    memory.Write(100, 123);

    auto result = applier(&state, &memory);
    ASSERT_EQ(2, result.GetRegisterWrites().size());
    auto hlWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::HL, hlWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(99, hlWrite.GetWordValue());
    auto aWrite = result.GetRegisterWrites()[1];
    ASSERT_EQ(RegisterName::A, aWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(123, aWrite.GetByteValue());

    PASS();
}

TEST Instr_JrNzS8_Jump()
{
    using namespace gb4e;
    auto applier = JrNFlagS8<0b10000000>;
    GbCpuState state;
    MemoryStateFake memory;
    auto regPC = GetRegister(RegisterName::PC);
    state.Set16BitRegisterValue(regPC, 0);
    memory.Write(0x0, 0x20);
    memory.Write(0x1, 100);
    state.SetFlags(0b00000000);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::PC, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0, regWrite.GetWordPreviousValue());
    ASSERT_EQ(100, regWrite.GetWordValue());
    ASSERT_EQ(2, result.GetConsumedBytes());
    ASSERT_EQ(3, result.GetConsumedCycles());

    PASS();
}

TEST Instr_JrNzS8_NoJump()
{
    using namespace gb4e;
    auto applier = JrNFlagS8<0b10000000>;
    GbCpuState state;
    MemoryStateFake memory;
    auto regPC = GetRegister(RegisterName::PC);
    state.Set16BitRegisterValue(regPC, 0);
    memory.Write(0x0, 0x20);
    memory.Write(0x1, 0xBA);
    state.SetFlags(0b10000000);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT(result.GetRegisterWrites().empty());
    ASSERT_EQ(2, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());

    PASS();
}

TEST Instr_LdHlPlus_A()
{
    using namespace gb4e;
    auto applier = LdHlIncDecA<1>;
    GbCpuState state;
    MemoryStateFake memory;
    auto regHL = GetRegister(RegisterName::HL);
    state.Set16BitRegisterValue(regHL, 0x1);
    memory.Write(0x1, 100);
    state.Set8BitRegisterValue(GetRegister(RegisterName::A), 123);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT_EQ(1, result.GetMemoryWrites().size());
    auto memWrite = result.GetMemoryWrites()[0];
    ASSERT_EQ(0x1, memWrite.GetLocation());
    ASSERT_EQ(100, memWrite.GetPreviousValue());
    ASSERT_EQ(123, memWrite.GetValue());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::HL, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0x1, regWrite.GetWordPreviousValue());
    ASSERT_EQ(0x2, regWrite.GetWordValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());

    PASS();
}

TEST Instr_LdHlMinus_A()
{
    using namespace gb4e;
    auto applier = LdHlIncDecA<-1>;
    GbCpuState state;
    MemoryStateFake memory;
    auto regHL = GetRegister(RegisterName::HL);
    state.Set16BitRegisterValue(regHL, 0x1);
    memory.Write(0x1, 100);
    state.Set8BitRegisterValue(GetRegister(RegisterName::A), 123);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT_EQ(1, result.GetMemoryWrites().size());
    auto memWrite = result.GetMemoryWrites()[0];
    ASSERT_EQ(0x1, memWrite.GetLocation());
    ASSERT_EQ(100, memWrite.GetPreviousValue());
    ASSERT_EQ(123, memWrite.GetValue());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::HL, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0x1, regWrite.GetWordPreviousValue());
    ASSERT_EQ(0x0, regWrite.GetWordValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());

    PASS();
}

TEST Instr_LdMemViaReg_C_A()
{
    using namespace gb4e;
    auto applier = LdMemViaReg<RegisterName::C, RegisterName::A>;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set8BitRegisterValue(GetRegister(RegisterName::C), 0xBA);
    state.Set8BitRegisterValue(GetRegister(RegisterName::A), 123);
    memory.Write(0xFFBA, 100);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT_EQ(1, result.GetMemoryWrites().size());
    auto memWrite = result.GetMemoryWrites()[0];
    ASSERT_EQ(0xFFBA, memWrite.GetLocation());
    ASSERT_EQ(100, memWrite.GetPreviousValue());
    ASSERT_EQ(123, memWrite.GetValue());
    ASSERT(result.GetRegisterWrites().empty());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());
    PASS();
}

TEST Instr_LdMemViaReg_HL_B()
{
    using namespace gb4e;
    auto applier = LdMemViaReg<RegisterName::HL, RegisterName::B>;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set16BitRegisterValue(GetRegister(RegisterName::HL), 0xBEEF);
    state.Set8BitRegisterValue(GetRegister(RegisterName::B), 123);
    memory.Write(0xBEEF, 100);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT_EQ(1, result.GetMemoryWrites().size());
    auto memWrite = result.GetMemoryWrites()[0];
    ASSERT_EQ(0xBEEF, memWrite.GetLocation());
    ASSERT_EQ(100, memWrite.GetPreviousValue());
    ASSERT_EQ(123, memWrite.GetValue());
    ASSERT(result.GetRegisterWrites().empty());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());
    PASS();
}

TEST Instr_JrZS8_Jump()
{
    using namespace gb4e;
    auto applier = JrFlagS8<0b10000000>;
    GbCpuState state;
    MemoryStateFake memory;
    auto regPC = GetRegister(RegisterName::PC);
    state.Set16BitRegisterValue(regPC, 0);
    memory.Write(0x0, 0x20);
    memory.Write(0x1, 100);
    state.SetFlags(0b10000000);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::PC, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0, regWrite.GetWordPreviousValue());
    ASSERT_EQ(100, regWrite.GetWordValue());
    ASSERT_EQ(2, result.GetConsumedBytes());
    ASSERT_EQ(3, result.GetConsumedCycles());

    PASS();
}

TEST Instr_JrZS8_NoJump()
{
    using namespace gb4e;
    auto applier = JrFlagS8<0b10000000>;
    GbCpuState state;
    MemoryStateFake memory;
    auto regPC = GetRegister(RegisterName::PC);
    state.Set16BitRegisterValue(regPC, 0);
    memory.Write(0x0, 0x20);
    memory.Write(0x1, 100);
    state.SetFlags(0b00000000);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT(result.GetRegisterWrites().empty());
    ASSERT_EQ(2, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());

    PASS();
}

TEST Instr_Add_A_B()
{
    using namespace gb4e;
    auto applier = Add<RegisterName::A, RegisterName::B>;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set8BitRegisterValue(GetRegister(RegisterName::A), 100);
    state.Set8BitRegisterValue(GetRegister(RegisterName::B), 23);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(100, regWrite.GetBytePreviousValue());
    ASSERT_EQ(123, regWrite.GetByteValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(1, result.GetConsumedCycles());
    PASS();
}

TEST Instr_Add_HL_BC()
{
    using namespace gb4e;
    auto applier = Add<RegisterName::HL, RegisterName::BC>;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set16BitRegisterValue(GetRegister(RegisterName::HL), 100);
    state.Set16BitRegisterValue(GetRegister(RegisterName::BC), 23);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::HL, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(100, regWrite.GetWordPreviousValue());
    ASSERT_EQ(123, regWrite.GetWordValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());
    PASS();
}

TEST Instr_AddFromAddrReg_A_HL()
{
    using namespace gb4e;
    auto applier = AddFromAddrReg<RegisterName::A, RegisterName::HL>;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set8BitRegisterValue(GetRegister(RegisterName::A), 100);
    state.Set16BitRegisterValue(GetRegister(RegisterName::HL), 0x1);
    memory.Write(0x1, 23);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(100, regWrite.GetBytePreviousValue());
    ASSERT_EQ(123, regWrite.GetByteValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());
    PASS();
}

TEST Instr_Sub_B()
{
    using namespace gb4e;
    auto applier = Sub<RegisterName::B>;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set8BitRegisterValue(GetRegister(RegisterName::A), 100);
    state.Set8BitRegisterValue(GetRegister(RegisterName::B), 10);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(100, regWrite.GetBytePreviousValue());
    ASSERT_EQ(90, regWrite.GetByteValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(1, result.GetConsumedCycles());
    PASS();
}

TEST Instr_And_B()
{
    using namespace gb4e;
    auto applier = And<RegisterName::B>;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set8BitRegisterValue(GetRegister(RegisterName::A), 0b10010000);
    state.Set8BitRegisterValue(GetRegister(RegisterName::B), 0b10000001);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0b10010000, regWrite.GetBytePreviousValue());
    ASSERT_EQ(0b10000000, regWrite.GetByteValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(1, result.GetConsumedCycles());
    PASS();
}

TEST Instr_Xor_B()
{
    using namespace gb4e;
    auto applier = Xor<RegisterName::B>;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set8BitRegisterValue(GetRegister(RegisterName::A), 0b10010000);
    state.Set8BitRegisterValue(GetRegister(RegisterName::B), 0b10000001);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0b10010000, regWrite.GetBytePreviousValue());
    ASSERT_EQ(0b00010001, regWrite.GetByteValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(1, result.GetConsumedCycles());
    PASS();
}

TEST Instr_Xor_HL()
{
    using namespace gb4e;
    auto applier = Xor<RegisterName::HL>;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set8BitRegisterValue(GetRegister(RegisterName::A), 0b10010000);
    state.Set16BitRegisterValue(Register(RegisterName::HL), 0x100);
    memory.Write(0x100, 0b10000001);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0b10010000, regWrite.GetBytePreviousValue());
    ASSERT_EQ(0b00010001, regWrite.GetByteValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());
    PASS();
}

TEST Instr_Or_B()
{
    using namespace gb4e;
    auto applier = Or<RegisterName::B>;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set8BitRegisterValue(GetRegister(RegisterName::A), 0b10010000);
    state.Set8BitRegisterValue(GetRegister(RegisterName::B), 0b10000001);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0b10010000, regWrite.GetBytePreviousValue());
    ASSERT_EQ(0b10010001, regWrite.GetByteValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(1, result.GetConsumedCycles());
    PASS();
}

TEST Instr_CpFromMem_HL_Equals()
{
    using namespace gb4e;
    auto applier = CpFromMem<RegisterName::HL>;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(0);
    state.Set8BitRegisterValue(GetRegister(RegisterName::A), 100);
    state.Set16BitRegisterValue(GetRegister(RegisterName::HL), 0x1);
    memory.Write(0x1, 100);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    auto flagSet = result.GetFlagSet().value();
    ASSERT_EQ(0, flagSet.GetPreviousValue());
    ASSERT_EQ(0b11000000, flagSet.GetValue());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT(result.GetRegisterWrites().empty());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());
    PASS();
}

TEST Instr_CpFromMem_HL_NotEquals()
{
    using namespace gb4e;
    auto applier = CpFromMem<RegisterName::HL>;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(0);
    state.Set8BitRegisterValue(GetRegister(RegisterName::A), 100);
    state.Set16BitRegisterValue(GetRegister(RegisterName::HL), 0x1);
    memory.Write(0x1, 23);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    auto flagSet = result.GetFlagSet().value();
    ASSERT_EQ(0, flagSet.GetPreviousValue());
    ASSERT_EQ(0b01000000, flagSet.GetValue());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT(result.GetRegisterWrites().empty());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());
    PASS();
}

TEST Instr_RetNz_Jump()
{
    using namespace gb4e;
    auto applier = RetNFlag<0b10000000>;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(0);
    auto sp = GetRegister(RegisterName::SP);
    auto pc = GetRegister(RegisterName::PC);
    state.Set16BitRegisterValue(sp, 0x10);
    state.Set16BitRegisterValue(pc, 0x01);
    memory.Write(0x10, 0xEF);
    memory.Write(0x11, 0xBE);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(2, result.GetRegisterWrites().size());
    auto regWriteSp = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::SP, regWriteSp.GetRegister().GetRegisterName());
    ASSERT_EQ(0x10, regWriteSp.GetWordPreviousValue());
    ASSERT_EQ(0x12, regWriteSp.GetWordValue());
    auto regWritePc = result.GetRegisterWrites()[1];
    ASSERT_EQ(RegisterName::PC, regWritePc.GetRegister().GetRegisterName());
    ASSERT_EQ(0x01, regWritePc.GetWordPreviousValue());
    ASSERT_EQ(0xBEEF, regWritePc.GetWordValue());
    ASSERT_EQ(0, result.GetConsumedBytes());
    ASSERT_EQ(5, result.GetConsumedCycles());
    PASS();
}

TEST Instr_RetNz_NoJump()
{
    using namespace gb4e;
    auto applier = RetNFlag<0b10000000>;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(0b10000000);
    auto sp = GetRegister(RegisterName::SP);
    auto pc = GetRegister(RegisterName::PC);
    state.Set16BitRegisterValue(sp, 0x10);
    state.Set16BitRegisterValue(pc, 0x01);
    memory.Write(0x10, 0xEF);
    memory.Write(0x11, 0xBE);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT(result.GetRegisterWrites().empty());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());
    PASS();
}

TEST Instr_RetZ_Jump()
{
    using namespace gb4e;
    auto applier = RetFlag<0b10000000>;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(0b10000000);
    auto sp = GetRegister(RegisterName::SP);
    auto pc = GetRegister(RegisterName::PC);
    state.Set16BitRegisterValue(sp, 0x10);
    state.Set16BitRegisterValue(pc, 0x01);
    memory.Write(0x10, 0xEF);
    memory.Write(0x11, 0xBE);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(2, result.GetRegisterWrites().size());
    auto regWriteSp = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::SP, regWriteSp.GetRegister().GetRegisterName());
    ASSERT_EQ(0x10, regWriteSp.GetWordPreviousValue());
    ASSERT_EQ(0x12, regWriteSp.GetWordValue());
    auto regWritePc = result.GetRegisterWrites()[1];
    ASSERT_EQ(RegisterName::PC, regWritePc.GetRegister().GetRegisterName());
    ASSERT_EQ(0x01, regWritePc.GetWordPreviousValue());
    ASSERT_EQ(0xBEEF, regWritePc.GetWordValue());
    ASSERT_EQ(0, result.GetConsumedBytes());
    ASSERT_EQ(5, result.GetConsumedCycles());
    PASS();
}

TEST Instr_RetZ_NoJump()
{
    using namespace gb4e;
    auto applier = RetFlag<0b10000000>;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(0);
    auto sp = GetRegister(RegisterName::SP);
    auto pc = GetRegister(RegisterName::PC);
    state.Set16BitRegisterValue(sp, 0x10);
    state.Set16BitRegisterValue(pc, 0x01);
    memory.Write(0x10, 0xEF);
    memory.Write(0x11, 0xBE);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT(result.GetRegisterWrites().empty());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());
    PASS();
}

TEST Instr_Pop_BC()
{
    using namespace gb4e;
    auto applier = Pop<RegisterName::BC>;
    GbCpuState state;
    MemoryStateFake memory;
    auto bc = GetRegister(RegisterName::BC);
    auto sp = GetRegister(RegisterName::SP);
    state.Set16BitRegisterValue(bc, 100);
    state.Set16BitRegisterValue(sp, 0x10);
    memory.Write(0x10, 0xEF);
    memory.Write(0x11, 0xBE);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(2, result.GetRegisterWrites().size());
    auto regWriteSp = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::SP, regWriteSp.GetRegister().GetRegisterName());
    ASSERT_EQ(0x10, regWriteSp.GetWordPreviousValue());
    ASSERT_EQ(0x12, regWriteSp.GetWordValue());
    auto regWriteBc = result.GetRegisterWrites()[1];
    ASSERT_EQ(RegisterName::BC, regWriteBc.GetRegister().GetRegisterName());
    ASSERT_EQ(100, regWriteBc.GetWordPreviousValue());
    ASSERT_EQ(0xBEEF, regWriteBc.GetWordValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(3, result.GetConsumedCycles());
    PASS();
}

TEST Instr_Push_BC()
{
    using namespace gb4e;
    auto applier = Push<RegisterName::BC>;
    GbCpuState state;
    MemoryStateFake memory;
    auto bc = GetRegister(RegisterName::BC);
    auto sp = GetRegister(RegisterName::SP);
    state.Set16BitRegisterValue(bc, 0xBEEF);
    state.Set16BitRegisterValue(sp, 0x12);
    memory.Write(0x11, 0xFE);
    memory.Write(0x10, 0xCA);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT_EQ(2, result.GetMemoryWrites().size());
    auto memWriteHi = result.GetMemoryWrites()[0];
    ASSERT_EQ(0x11, memWriteHi.GetLocation());
    ASSERT_EQ(0xFE, memWriteHi.GetPreviousValue());
    ASSERT_EQ(0xBE, memWriteHi.GetValue());
    auto memWriteLo = result.GetMemoryWrites()[1];
    ASSERT_EQ(0x10, memWriteLo.GetLocation());
    ASSERT_EQ(0xCA, memWriteLo.GetPreviousValue());
    ASSERT_EQ(0xEF, memWriteLo.GetValue());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWriteSp = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::SP, regWriteSp.GetRegister().GetRegisterName());
    ASSERT_EQ(0x12, regWriteSp.GetWordPreviousValue());
    ASSERT_EQ(0x10, regWriteSp.GetWordValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(4, result.GetConsumedCycles());
    PASS();
}

TEST Instr_Ret()
{
    using namespace gb4e;
    auto applier = Ret;
    GbCpuState state;
    MemoryStateFake memory;
    auto sp = GetRegister(RegisterName::SP);
    auto pc = GetRegister(RegisterName::PC);
    state.Set16BitRegisterValue(sp, 0x10);
    state.Set16BitRegisterValue(pc, 0x01);
    memory.Write(0x10, 0xEF);
    memory.Write(0x11, 0xBE);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(2, result.GetRegisterWrites().size());
    auto regWriteSp = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::SP, regWriteSp.GetRegister().GetRegisterName());
    ASSERT_EQ(0x10, regWriteSp.GetWordPreviousValue());
    ASSERT_EQ(0x12, regWriteSp.GetWordValue());
    auto regWritePc = result.GetRegisterWrites()[1];
    ASSERT_EQ(RegisterName::PC, regWritePc.GetRegister().GetRegisterName());
    ASSERT_EQ(0x01, regWritePc.GetWordPreviousValue());
    ASSERT_EQ(0xBEEF, regWritePc.GetWordValue());
    ASSERT_EQ(0, result.GetConsumedBytes());
    ASSERT_EQ(4, result.GetConsumedCycles());
    PASS();
}

TEST Instr_CallA16()
{
    using namespace gb4e;
    auto applier = CallA16;
    GbCpuState state;
    MemoryStateFake memory;
    auto sp = GetRegister(RegisterName::SP);
    auto pc = GetRegister(RegisterName::PC);
    state.Set16BitRegisterValue(sp, 0x12);
    state.Set16BitRegisterValue(pc, 0x01);
    memory.Write(0x01, 0xCD);
    memory.Write(0x02, 0xEF);
    memory.Write(0x03, 0xBE);

    memory.Write(0x10, 0xBE);
    memory.Write(0x11, 0xBA);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT_EQ(2, result.GetMemoryWrites().size());
    auto memWriteHi = result.GetMemoryWrites()[0];
    ASSERT_EQ(0x11, memWriteHi.GetLocation());
    ASSERT_EQ(0xBA, memWriteHi.GetPreviousValue());
    ASSERT_EQ(0x00, memWriteHi.GetValue());
    auto memWriteLo = result.GetMemoryWrites()[1];
    ASSERT_EQ(0x10, memWriteLo.GetLocation());
    ASSERT_EQ(0xBE, memWriteLo.GetPreviousValue());
    ASSERT_EQ(0x04, memWriteLo.GetValue());
    ASSERT_EQ(2, result.GetRegisterWrites().size());
    auto regWriteSp = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::SP, regWriteSp.GetRegister().GetRegisterName());
    ASSERT_EQ(0x12, regWriteSp.GetWordPreviousValue());
    ASSERT_EQ(0x10, regWriteSp.GetWordValue());
    auto regWritePc = result.GetRegisterWrites()[1];
    ASSERT_EQ(RegisterName::PC, regWritePc.GetRegister().GetRegisterName());
    ASSERT_EQ(0x01, regWritePc.GetWordPreviousValue());
    ASSERT_EQ(0xBEEF, regWritePc.GetWordValue());
    ASSERT_EQ(0, result.GetConsumedBytes());
    ASSERT_EQ(6, result.GetConsumedCycles());
    PASS();
}

TEST Instr_LdA8()
{
    using namespace gb4e;
    auto applier = LdA8;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set8BitRegisterValue(GetRegister(RegisterName::A), 123);
    state.Set16BitRegisterValue(GetRegister(RegisterName::PC), 0x1);
    memory.Write(0x1, 0xE1);
    memory.Write(0x2, 0x10);
    memory.Write(0xFF10, 100);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT_EQ(1, result.GetMemoryWrites().size());
    auto memWrite = result.GetMemoryWrites()[0];
    ASSERT_EQ(0xFF10, memWrite.GetLocation());
    ASSERT_EQ(100, memWrite.GetPreviousValue());
    ASSERT_EQ(123, memWrite.GetValue());
    ASSERT(result.GetRegisterWrites().empty());
    ASSERT_EQ(2, result.GetConsumedBytes());
    ASSERT_EQ(3, result.GetConsumedCycles());
    PASS();
}

TEST Instr_LdA16_A()
{
    using namespace gb4e;
    auto applier = LdA16<RegisterName::A>;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set8BitRegisterValue(GetRegister(RegisterName::A), 123);
    state.Set16BitRegisterValue(GetRegister(RegisterName::PC), 0x1);
    memory.Write(0x1, 0xEA);
    memory.Write(0x2, 0xEF);
    memory.Write(0x3, 0xBE);
    memory.Write(0xBEEF, 100);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT_EQ(1, result.GetMemoryWrites().size());
    auto memWrite = result.GetMemoryWrites()[0];
    ASSERT_EQ(0xBEEF, memWrite.GetLocation());
    ASSERT_EQ(100, memWrite.GetPreviousValue());
    ASSERT_EQ(123, memWrite.GetValue());
    ASSERT(result.GetRegisterWrites().empty());
    ASSERT_EQ(3, result.GetConsumedBytes());
    ASSERT_EQ(4, result.GetConsumedCycles());
    PASS();
}

TEST Instr_LdA16_SP()
{
    using namespace gb4e;
    auto applier = LdA16<RegisterName::SP>;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set16BitRegisterValue(GetRegister(RegisterName::SP), 0xBABE);
    state.Set16BitRegisterValue(GetRegister(RegisterName::PC), 0x1);
    memory.Write(0x1, 0xEA);
    memory.Write(0x2, 0xEF);
    memory.Write(0x3, 0xBE);
    memory.Write(0xBEEF, 100);
    memory.Write(0xBEEF + 1, 101);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT_EQ(2, result.GetMemoryWrites().size());
    auto memWriteLo = result.GetMemoryWrites()[0];
    ASSERT_EQ(0xBEEF, memWriteLo.GetLocation());
    ASSERT_EQ(100, memWriteLo.GetPreviousValue());
    ASSERT_EQ(0xBE, memWriteLo.GetValue());
    auto memWriteHi = result.GetMemoryWrites()[1];
    ASSERT_EQ(0xBEEF + 1, memWriteHi.GetLocation());
    ASSERT_EQ(101, memWriteHi.GetPreviousValue());
    ASSERT_EQ(0xBA, memWriteHi.GetValue());
    ASSERT(result.GetRegisterWrites().empty());
    ASSERT_EQ(3, result.GetConsumedBytes());
    ASSERT_EQ(5, result.GetConsumedCycles());
    PASS();
}

TEST Instr_LDFromA8_A()
{
    using namespace gb4e;
    auto applier = LdFromA8<RegisterName::A>;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set8BitRegisterValue(GetRegister(RegisterName::A), 100);
    state.Set16BitRegisterValue(GetRegister(RegisterName::PC), 0x1);
    memory.Write(0x1, 0xF0);
    memory.Write(0x2, 0x10);
    memory.Write(0xFF10, 123);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(100, regWrite.GetBytePreviousValue());
    ASSERT_EQ(123, regWrite.GetByteValue());
    ASSERT_EQ(2, result.GetConsumedBytes());
    ASSERT_EQ(3, result.GetConsumedCycles());
    PASS();
}

TEST Instr_LdFromA16_A()
{
    using namespace gb4e;
    auto applier = LdFromA16<RegisterName::A>;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set8BitRegisterValue(GetRegister(RegisterName::A), 100);
    state.Set16BitRegisterValue(GetRegister(RegisterName::PC), 0x1);
    memory.Write(0x1, 0xF0);
    memory.Write(0x2, 0xEF);
    memory.Write(0x3, 0xBE);
    memory.Write(0xBEEF, 123);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(100, regWrite.GetBytePreviousValue());
    ASSERT_EQ(123, regWrite.GetByteValue());
    ASSERT_EQ(3, result.GetConsumedBytes());
    ASSERT_EQ(4, result.GetConsumedCycles());
    PASS();
}

TEST Instr_CpD8_Equals()
{
    using namespace gb4e;
    auto applier = CpD8;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(0);
    state.Set8BitRegisterValue(GetRegister(RegisterName::A), 100);
    state.Set16BitRegisterValue(GetRegister(RegisterName::PC), 0x1);
    memory.Write(0x1, 0xFE);
    memory.Write(0x2, 100);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    auto flagSet = result.GetFlagSet().value();
    ASSERT_EQ(0, flagSet.GetPreviousValue());
    ASSERT_EQ(0b11000000, flagSet.GetValue());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT(result.GetRegisterWrites().empty());
    ASSERT_EQ(2, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());
    PASS();
}

TEST Instr_CpD8_NotEquals()
{
    using namespace gb4e;
    auto applier = CpD8;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(0);
    state.Set8BitRegisterValue(GetRegister(RegisterName::A), 100);
    state.Set16BitRegisterValue(GetRegister(RegisterName::PC), 0x1);
    memory.Write(0x1, 0xFE);
    memory.Write(0x2, 101);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    auto flagSet = result.GetFlagSet().value();
    ASSERT_EQ(0, flagSet.GetPreviousValue());
    ASSERT_EQ(0b01000000, flagSet.GetValue());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT(result.GetRegisterWrites().empty());
    ASSERT_EQ(2, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());
    PASS();
}

TEST Instr_Rl_C_Carry()
{
    using namespace gb4e;
    auto applier = Rl<RegisterName::C>;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(0b00010000);
    state.Set8BitRegisterValue(GetRegister(RegisterName::C), 0b10000000);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    auto flagSet = result.GetFlagSet().value();
    ASSERT_EQ(0b00010000, flagSet.GetPreviousValue());
    ASSERT_EQ(0b00010000, flagSet.GetValue());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::C, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0b10000000, regWrite.GetBytePreviousValue());
    ASSERT_EQ(0b00000001, regWrite.GetByteValue());
    ASSERT_EQ(2, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());
    PASS();
}

TEST Instr_Rl_C_NoCarry()
{
    using namespace gb4e;
    auto applier = Rl<RegisterName::C>;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(0b00010000);
    state.Set8BitRegisterValue(GetRegister(RegisterName::C), 0b01000000);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    auto flagSet = result.GetFlagSet().value();
    ASSERT_EQ(0b00010000, flagSet.GetPreviousValue());
    ASSERT_EQ(0b00000000, flagSet.GetValue());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::C, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0b01000000, regWrite.GetBytePreviousValue());
    ASSERT_EQ(0b10000001, regWrite.GetByteValue());
    ASSERT_EQ(2, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());
    PASS();
}

TEST Instr_Rl_C_Zero()
{
    using namespace gb4e;
    auto applier = Rl<RegisterName::C>;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(0);
    state.Set8BitRegisterValue(GetRegister(RegisterName::C), 0);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    auto flagSet = result.GetFlagSet().value();
    ASSERT_EQ(0b10000000, flagSet.GetValue());
    PASS();
}

TEST Instr_Bit_7_H_0()
{
    using namespace gb4e;
    auto applier = Bit<7, RegisterName::H>;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(0b00010000);
    state.Set8BitRegisterValue(GetRegister(RegisterName::H), 0b00000000);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    auto flagSet = result.GetFlagSet().value();
    ASSERT_EQ(0b00010000, flagSet.GetPreviousValue());
    ASSERT_EQ(0b10110000, flagSet.GetValue());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT(result.GetRegisterWrites().empty());
    ASSERT_EQ(2, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());
    PASS();
}

TEST Instr_Bit_7_H_1()
{
    using namespace gb4e;
    auto applier = Bit<7, RegisterName::H>;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(0b00010000);
    state.Set8BitRegisterValue(GetRegister(RegisterName::H), 0b10000000);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    auto flagSet = result.GetFlagSet().value();
    ASSERT_EQ(0b00010000, flagSet.GetPreviousValue());
    ASSERT_EQ(0b00110000, flagSet.GetValue());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT(result.GetRegisterWrites().empty());
    ASSERT_EQ(2, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());
    PASS();
}

TEST Instr_Bit_7_H_FromBootrom()
{
    // This test should mimic the state the CPU is in when executing the 5th instruction in the bootrom
    using namespace gb4e;
    auto applier = Bit<7, RegisterName::H>;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(0x80);
    state.Set8BitRegisterValue(GetRegister(RegisterName::H), 0x9f);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    auto flagSet = result.GetFlagSet().value();
    ASSERT_EQ(0b00100000, flagSet.GetValue());

    PASS();
}

TEST Instr_JrNzS8_FromBootrom()
{
    // This test should mimic the state the CPU is in when executing the 6th instruction in the bootrom
    using namespace gb4e;
    auto applier = JrNFlagS8<0b10000000>;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set16BitRegisterValue(GetRegister(RegisterName::PC), 0x000a);
    state.SetFlags(0b00100000);
    memory.Write(0x000a, 0x20);
    memory.Write(0x000b, 0xFB);

    auto result = applier(&state, &memory);

    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::PC, regWrite.GetRegister().GetRegisterName());
    // The instruction jumps -5 (0xFB twos complement) and then +2 due to consumedbytes being 2
    ASSERT_EQ(0x0005, regWrite.GetWordValue());

    PASS();
}

TEST Instr_Bit_7_H_FromBootrom_Break()
{
    // This test should mimic the state the CPU is in when exiting the first loop in the bootrom
    using namespace gb4e;
    auto applier = Bit<7, RegisterName::H>;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(0x20);
    state.Set16BitRegisterValue(GetRegister(RegisterName::HL), 0x7fff);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    auto flagSet = result.GetFlagSet().value();
    ASSERT_EQ(0b10100000, flagSet.GetValue());

    PASS();
}

TEST Instr_JpA16()
{
    using namespace gb4e;
    auto applier = JpA16;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set16BitRegisterValue(Register(RegisterName::PC), 0x101);
    memory.Write(0x101, 0xC3);
    memory.Write(0x102, 0x50);
    memory.Write(0x103, 0x01);

    auto result = applier(&state, &memory);

    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::PC, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0x150, regWrite.GetWordValue());
    PASS();
}

TEST Instr_Di()
{
    using namespace gb4e;
    auto applier = ToggleInterrupts<false>;
    GbCpuState state;
    MemoryStateFake memory;

    auto result = applier(&state, &memory);

    ASSERT(result.GetInterruptSet().has_value());
    auto interruptSet = result.GetInterruptSet().value();
    ASSERT_EQ(false, interruptSet.GetValue());
    ASSERT_EQ(false, interruptSet.GetWithInstructionDelay());
    PASS();
}

TEST Instr_Ei()
{
    using namespace gb4e;
    auto applier = ToggleInterrupts<true>;
    GbCpuState state;
    MemoryStateFake memory;

    auto result = applier(&state, &memory);

    ASSERT(result.GetInterruptSet().has_value());
    auto interruptSet = result.GetInterruptSet().value();
    ASSERT_EQ(true, interruptSet.GetValue());
    ASSERT_EQ(true, interruptSet.GetWithInstructionDelay());
    PASS();
}

TEST Instr_Reti()
{
    using namespace gb4e;
    auto applier = Reti;
    GbCpuState state;
    MemoryStateFake memory;
    auto sp = GetRegister(RegisterName::SP);
    auto pc = GetRegister(RegisterName::PC);
    state.Set16BitRegisterValue(sp, 0x10);
    state.Set16BitRegisterValue(pc, 0x01);
    memory.Write(0x10, 0xEF);
    memory.Write(0x11, 0xBE);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(2, result.GetRegisterWrites().size());
    ASSERT(result.GetInterruptSet().has_value());
    auto interruptSet = result.GetInterruptSet().value();
    ASSERT_EQ(true, interruptSet.GetValue());
    ASSERT_EQ(false, interruptSet.GetWithInstructionDelay());
    PASS();
}

TEST Instr_LdHlD8()
{
    using namespace gb4e;
    auto applier = LdHlD8;
    GbCpuState state;
    MemoryStateFake memory;
    memory.Write(0x0250, 0x36);
    memory.Write(0x0251, 0x77);
    state.Set16BitRegisterValue(Register(RegisterName::PC), 0x0250);
    state.Set16BitRegisterValue(Register(RegisterName::HL), 0xFF24);

    auto result = applier(&state, &memory);

    ASSERT_EQ(1, result.GetMemoryWrites().size());
    auto memWrite = result.GetMemoryWrites()[0];
    ASSERT_EQ(0xFF24, memWrite.GetLocation());
    ASSERT_EQ(0x77, memWrite.GetValue());
    ASSERT_EQ(2, result.GetConsumedBytes());
    ASSERT_EQ(3, result.GetConsumedCycles());

    PASS();
}

TEST Instr_Cpl()
{
    using namespace gb4e;
    auto applier = Cpl;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(0b10010000);
    state.Set8BitRegisterValue(Register(RegisterName::A), 0b10011111);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT_EQ(0b11110000, result.GetFlagSet().value().GetValue()); // Preserve Z and C, set N and H to 1
    ASSERT_FALSE(result.GetInterruptSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0b01100000, regWrite.GetByteValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(1, result.GetConsumedCycles());
    PASS();
}

TEST Instr_AndD8()
{
    using namespace gb4e;
    auto applier = AndD8;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set16BitRegisterValue(Register(RegisterName::PC), 0x100);
    state.Set8BitRegisterValue(Register(RegisterName::A), 0x9F);
    memory.Write(0x100, 0xE6);
    memory.Write(0x101, 0xF0);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT_EQ(0b00100000, result.GetFlagSet().value().GetValue());
    ASSERT_FALSE(result.GetInterruptSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0x90, regWrite.GetByteValue());
    ASSERT_EQ(2, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());
    PASS();
}

TEST Instr_AndD8_0()
{
    using namespace gb4e;
    auto applier = AndD8;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set16BitRegisterValue(Register(RegisterName::PC), 0x100);
    state.Set8BitRegisterValue(Register(RegisterName::A), 0x9F);
    memory.Write(0x100, 0xE6);
    memory.Write(0x101, 0x00);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT_EQ(0b10100000, result.GetFlagSet().value().GetValue());
    PASS();
}

TEST Instr_Swap_A()
{
    using namespace gb4e;
    auto applier = Swap<RegisterName::A>;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(0xF0);
    state.Set8BitRegisterValue(Register(RegisterName::A), 0xF1);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT_EQ(0, result.GetFlagSet().value().GetValue());
    ASSERT_FALSE(result.GetInterruptSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0x1F, regWrite.GetByteValue());
    ASSERT_EQ(2, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());

    PASS();
}

TEST Instr_Swap_A_0()
{
    using namespace gb4e;
    auto applier = Swap<RegisterName::A>;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(0xF0);
    state.Set8BitRegisterValue(Register(RegisterName::A), 0x00);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT_EQ(0b10000000, result.GetFlagSet().value().GetValue());

    PASS();
}

TEST Instr_Swap_HL()
{
    using namespace gb4e;
    auto applier = Swap<RegisterName::HL>;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(0xF0);
    state.Set16BitRegisterValue(Register(RegisterName::HL), 0x100);
    memory.Write(0x100, 0xF1);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT_EQ(0, result.GetFlagSet().value().GetValue());
    ASSERT_FALSE(result.GetInterruptSet().has_value());
    ASSERT_EQ(1, result.GetMemoryWrites().size());
    auto memWrite = result.GetMemoryWrites()[0];
    ASSERT_EQ(0x100, memWrite.GetLocation());
    ASSERT_EQ(0x1F, memWrite.GetValue());
    ASSERT(result.GetRegisterWrites().empty());
    ASSERT_EQ(2, result.GetConsumedBytes());
    ASSERT_EQ(4, result.GetConsumedCycles());

    PASS();
}

TEST Instr_Rst5()
{
    using namespace gb4e;
    auto applier = Rst<5>;
    GbCpuState state;
    MemoryStateFake memory;
    auto sp = GetRegister(RegisterName::SP);
    auto pc = GetRegister(RegisterName::PC);
    state.Set16BitRegisterValue(sp, 0x12);
    state.Set16BitRegisterValue(pc, 0x01);
    memory.Write(0x01, 0xEF);

    memory.Write(0x10, 0xBE);
    memory.Write(0x11, 0xBA);

    auto result = applier(&state, &memory);

    ASSERT(!result.GetFlagSet().has_value());
    ASSERT_EQ(2, result.GetMemoryWrites().size());
    auto memWriteHi = result.GetMemoryWrites()[0];
    ASSERT_EQ(0x11, memWriteHi.GetLocation());
    ASSERT_EQ(0xBA, memWriteHi.GetPreviousValue());
    ASSERT_EQ(0x00, memWriteHi.GetValue());
    auto memWriteLo = result.GetMemoryWrites()[1];
    ASSERT_EQ(0x10, memWriteLo.GetLocation());
    ASSERT_EQ(0xBE, memWriteLo.GetPreviousValue());
    ASSERT_EQ(0x02, memWriteLo.GetValue());
    ASSERT_EQ(2, result.GetRegisterWrites().size());
    auto regWriteSp = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::SP, regWriteSp.GetRegister().GetRegisterName());
    ASSERT_EQ(0x12, regWriteSp.GetWordPreviousValue());
    ASSERT_EQ(0x10, regWriteSp.GetWordValue());
    auto regWritePc = result.GetRegisterWrites()[1];
    ASSERT_EQ(RegisterName::PC, regWritePc.GetRegister().GetRegisterName());
    ASSERT_EQ(0x01, regWritePc.GetWordPreviousValue());
    ASSERT_EQ(0x28, regWritePc.GetWordValue());
    ASSERT_EQ(0, result.GetConsumedBytes());
    ASSERT_EQ(4, result.GetConsumedCycles());
    PASS();
}

TEST Instr_Adc_B()
{
    using namespace gb4e;
    auto applier = Adc<RegisterName::B>;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(FLAG_C);
    state.Set8BitRegisterValue(Register(RegisterName::A), 1);
    state.Set8BitRegisterValue(Register(RegisterName::B), 1);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT_EQ(0, result.GetFlagSet().value().GetValue());
    ASSERT(!result.GetInterruptSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(3, regWrite.GetByteValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());

    PASS();
}

TEST Instr_Adc_HL()
{
    using namespace gb4e;
    auto applier = Adc<RegisterName::HL>;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(0);
    state.Set8BitRegisterValue(Register(RegisterName::A), 1);
    state.Set16BitRegisterValue(Register(RegisterName::HL), 0x100);
    memory.Write(0x100, 1);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT_EQ(0, result.GetFlagSet().value().GetValue());
    ASSERT(!result.GetInterruptSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(2, regWrite.GetByteValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());

    PASS();
}

TEST Instr_AdcD8()
{
    using namespace gb4e;
    auto applier = AdcD8;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(0);
    state.Set8BitRegisterValue(Register(RegisterName::A), 1);
    state.Set16BitRegisterValue(Register(RegisterName::PC), 0x100);
    memory.Write(0x100, 0xCE);
    memory.Write(0x101, 1);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT_EQ(0, result.GetFlagSet().value().GetValue());
    ASSERT(!result.GetInterruptSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(2, regWrite.GetByteValue());
    ASSERT_EQ(2, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());

    PASS();
}

TEST Instr_Rlca()
{
    using namespace gb4e;
    auto applier = Rlca;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(0);
    state.Set8BitRegisterValue(Register(RegisterName::A), 0b11000000);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT_EQ(0b00010000, result.GetFlagSet().value().GetValue());
    ASSERT(!result.GetInterruptSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::A, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0b10000001, regWrite.GetByteValue());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(1, result.GetConsumedCycles());

    PASS();
}

TEST Instr_JpHl()
{
    using namespace gb4e;
    auto applier = JpHl;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set16BitRegisterValue(Register(RegisterName::HL), 0x100);
    state.Set16BitRegisterValue(Register(RegisterName::PC), 0x00);

    auto result = applier(&state, &memory);

    ASSERT_FALSE(result.GetFlagSet().has_value());
    ASSERT_FALSE(result.GetInterruptSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::PC, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0x100, regWrite.GetWordValue());
    ASSERT_EQ(0, result.GetConsumedBytes());
    ASSERT_EQ(1, result.GetConsumedCycles());

    PASS();
}

TEST Instr_Res_B()
{
    using namespace gb4e;
    auto applier = Res<1, RegisterName::B>;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set8BitRegisterValue(Register(RegisterName::B), 0b11111111);

    auto result = applier(&state, &memory);

    ASSERT_FALSE(result.GetFlagSet().has_value());
    ASSERT_FALSE(result.GetInterruptSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::B, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0b11111101, regWrite.GetByteValue());
    ASSERT_EQ(2, result.GetConsumedBytes());
    ASSERT_EQ(2, result.GetConsumedCycles());
    PASS();
}

TEST Instr_Res_HL()
{
    using namespace gb4e;
    auto applier = Res<2, RegisterName::HL>;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set16BitRegisterValue(Register(RegisterName::HL), 0x100);
    memory.Write(0x100, 0b11111111);

    auto result = applier(&state, &memory);

    ASSERT_FALSE(result.GetFlagSet().has_value());
    ASSERT_FALSE(result.GetInterruptSet().has_value());
    ASSERT_EQ(1, result.GetMemoryWrites().size());
    auto memWrite = result.GetMemoryWrites()[0];
    ASSERT_EQ(0x100, memWrite.GetLocation());
    ASSERT_EQ(0b11111011, memWrite.GetValue());
    ASSERT(result.GetRegisterWrites().empty());
    ASSERT_EQ(2, result.GetConsumedBytes());
    ASSERT_EQ(4, result.GetConsumedCycles());
    PASS();
}

TEST Instr_IncHlAddr()
{
    using namespace gb4e;
    auto applier = IncHlAddr;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set16BitRegisterValue(Register(RegisterName::HL), 0x100);
    memory.Write(0x100, 1);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT_FALSE(result.GetInterruptSet().has_value());
    ASSERT_EQ(1, result.GetMemoryWrites().size());
    auto memWrite = result.GetMemoryWrites()[0];
    ASSERT_EQ(0x100, memWrite.GetLocation());
    ASSERT_EQ(2, memWrite.GetValue());
    ASSERT(result.GetRegisterWrites().empty());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(3, result.GetConsumedCycles());
    PASS();
}

TEST Instr_JpFlagA16_Jump()
{
    using namespace gb4e;
    auto applier = JpFlagA16<0b10000000>;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(FLAG_ZERO);
    state.Set16BitRegisterValue(Register(RegisterName::PC), 0x100);
    memory.Write(0x100, 0xCA);
    memory.Write(0x101, 0xEF);
    memory.Write(0x102, 0xBE);

    auto result = applier(&state, &memory);

    ASSERT_FALSE(result.GetFlagSet().has_value());
    ASSERT_FALSE(result.GetInterruptSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::PC, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0x100, regWrite.GetWordPreviousValue());
    ASSERT_EQ(0xBEEF, regWrite.GetWordValue());
    ASSERT_EQ(0, result.GetConsumedBytes());
    ASSERT_EQ(4, result.GetConsumedCycles());

    PASS();
}

TEST Instr_JpFlagA16_NoJump()
{
    using namespace gb4e;
    auto applier = JpFlagA16<0b10000000>;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(0);
    state.Set16BitRegisterValue(Register(RegisterName::PC), 0x100);
    memory.Write(0x100, 0xCA);
    memory.Write(0x101, 0xEF);
    memory.Write(0x102, 0xBE);

    auto result = applier(&state, &memory);

    ASSERT_FALSE(result.GetFlagSet().has_value());
    ASSERT_FALSE(result.GetInterruptSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT(result.GetRegisterWrites().empty());
    ASSERT_EQ(3, result.GetConsumedBytes());
    ASSERT_EQ(3, result.GetConsumedCycles());
    PASS();
}

TEST Instr_JpNFlagA16_Jump()
{
    using namespace gb4e;
    auto applier = JpNFlagA16<0b10000000>;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(0);
    state.Set16BitRegisterValue(Register(RegisterName::PC), 0x100);
    memory.Write(0x100, 0xCA);
    memory.Write(0x101, 0xEF);
    memory.Write(0x102, 0xBE);

    auto result = applier(&state, &memory);

    ASSERT_FALSE(result.GetFlagSet().has_value());
    ASSERT_FALSE(result.GetInterruptSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(RegisterName::PC, regWrite.GetRegister().GetRegisterName());
    ASSERT_EQ(0x100, regWrite.GetWordPreviousValue());
    ASSERT_EQ(0xBEEF, regWrite.GetWordValue());
    ASSERT_EQ(0, result.GetConsumedBytes());
    ASSERT_EQ(4, result.GetConsumedCycles());
    PASS();
}

TEST Instr_JpNFlagA16_NoJump()
{
    using namespace gb4e;
    auto applier = JpNFlagA16<0b10000000>;
    GbCpuState state;
    MemoryStateFake memory;
    state.SetFlags(FLAG_ZERO);
    state.Set16BitRegisterValue(Register(RegisterName::PC), 0x100);
    memory.Write(0x100, 0xCA);
    memory.Write(0x101, 0xEF);
    memory.Write(0x102, 0xBE);

    auto result = applier(&state, &memory);

    ASSERT_FALSE(result.GetFlagSet().has_value());
    ASSERT_FALSE(result.GetInterruptSet().has_value());
    ASSERT(result.GetMemoryWrites().empty());
    ASSERT(result.GetRegisterWrites().empty());
    ASSERT_EQ(3, result.GetConsumedBytes());
    ASSERT_EQ(3, result.GetConsumedCycles());
    PASS();
}

TEST Instr_DecHlAddr()
{
    using namespace gb4e;
    auto applier = DecHlAddr;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set16BitRegisterValue(Register(RegisterName::HL), 0x100);
    memory.Write(0x100, 1);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    ASSERT_FALSE(result.GetInterruptSet().has_value());
    ASSERT_EQ(1, result.GetMemoryWrites().size());
    auto memWrite = result.GetMemoryWrites()[0];
    ASSERT_EQ(0x100, memWrite.GetLocation());
    ASSERT_EQ(0, memWrite.GetValue());
    ASSERT(result.GetRegisterWrites().empty());
    ASSERT_EQ(1, result.GetConsumedBytes());
    ASSERT_EQ(3, result.GetConsumedCycles());
    PASS();
}

TEST Instr_Daa_0()
{
    using namespace gb4e;
    auto applier = Daa;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set8BitRegisterValue(Register(RegisterName::A), 0);
    state.SetFlags(0);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    auto flagSet = result.GetFlagSet().value();
    ASSERT_EQ(FLAG_ZERO, flagSet.GetValue());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(0, regWrite.GetByteValue());
    PASS();
}

TEST Instr_Daa_1()
{
    using namespace gb4e;
    auto applier = Daa;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set8BitRegisterValue(Register(RegisterName::A), 0x01);
    state.SetFlags(0);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    auto flagSet = result.GetFlagSet().value();
    ASSERT_EQ(0, flagSet.GetValue());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(0x01, regWrite.GetByteValue());
    PASS();
}

TEST Instr_Daa_10()
{
    using namespace gb4e;
    auto applier = Daa;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set8BitRegisterValue(Register(RegisterName::A), 0x0A);
    state.SetFlags(0);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    auto flagSet = result.GetFlagSet().value();
    ASSERT_EQ(0, flagSet.GetValue());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(0b00010000, regWrite.GetByteValue());
    PASS();
}

TEST Instr_Daa_12()
{
    using namespace gb4e;
    auto applier = Daa;
    GbCpuState state;
    MemoryStateFake memory;
    state.Set8BitRegisterValue(Register(RegisterName::A), 0x0C);
    state.SetFlags(0);

    auto result = applier(&state, &memory);

    ASSERT(result.GetFlagSet().has_value());
    auto flagSet = result.GetFlagSet().value();
    ASSERT_EQ(0, flagSet.GetValue());
    ASSERT_EQ(1, result.GetRegisterWrites().size());
    auto regWrite = result.GetRegisterWrites()[0];
    ASSERT_EQ(0b00010010, regWrite.GetByteValue());
    PASS();
}

SUITE(Instruction_test)
{
    RUN_TEST(Instr_Ld_B_A);
    RUN_TEST(Instr_Ld_SP_D16);
    RUN_TEST(Instr_Inc_BC);
    RUN_TEST(Instr_Inc_BC_Wraparound);
    RUN_TEST(Instr_Inc_A);
    RUN_TEST(Instr_Inc_A_Wraparound);
    RUN_TEST(Instr_Inc_A_HalfCarry);
    RUN_TEST(Instr_Inc_A_PreservesCarry);
    RUN_TEST(Instr_Dec_BC);
    RUN_TEST(Instr_Dec_BC_Wraparound);
    RUN_TEST(Instr_Dec_A);
    RUN_TEST(Instr_Dec_A_Zero);
    RUN_TEST(Instr_Dec_A_Wraparound);
    RUN_TEST(Instr_Dec_A_PreservesCarry);
    RUN_TEST(Instr_LdD8_A);
    RUN_TEST(Instr_LdD16_BC);
    RUN_TEST(Instr_Rla_NoCarry);
    RUN_TEST(Instr_Rla_Carry);
    RUN_TEST(Instr_JrS8);
    RUN_TEST(Instr_LdFromAddrReg_A_DE);
    RUN_TEST(Instr_LdFromAddrReg_WithModifier);
    RUN_TEST(Instr_JrNzS8_Jump);
    RUN_TEST(Instr_JrNzS8_NoJump);
    RUN_TEST(Instr_LdHlPlus_A);
    RUN_TEST(Instr_LdHlMinus_A);
    RUN_TEST(Instr_LdMemViaReg_C_A);
    RUN_TEST(Instr_LdMemViaReg_HL_B);
    RUN_TEST(Instr_JrZS8_Jump);
    RUN_TEST(Instr_JrZS8_NoJump);
    RUN_TEST(Instr_Add_A_B);
    RUN_TEST(Instr_Add_HL_BC);
    RUN_TEST(Instr_AddFromAddrReg_A_HL);
    RUN_TEST(Instr_Sub_B);
    RUN_TEST(Instr_And_B);
    RUN_TEST(Instr_Xor_B);
    RUN_TEST(Instr_Xor_HL);
    RUN_TEST(Instr_Or_B);
    RUN_TEST(Instr_CpFromMem_HL_Equals);
    RUN_TEST(Instr_CpFromMem_HL_NotEquals);
    RUN_TEST(Instr_RetNz_Jump);
    RUN_TEST(Instr_RetNz_NoJump);
    RUN_TEST(Instr_RetZ_Jump);
    RUN_TEST(Instr_RetZ_NoJump);
    RUN_TEST(Instr_Pop_BC);
    RUN_TEST(Instr_Push_BC);
    RUN_TEST(Instr_Ret);
    RUN_TEST(Instr_CallA16);
    RUN_TEST(Instr_LdA8);
    RUN_TEST(Instr_LdA16_A);
    RUN_TEST(Instr_LdA16_SP);
    RUN_TEST(Instr_LDFromA8_A);
    RUN_TEST(Instr_LdFromA16_A);
    RUN_TEST(Instr_CpD8_Equals);
    RUN_TEST(Instr_CpD8_NotEquals);
    RUN_TEST(Instr_Rl_C_Carry);
    RUN_TEST(Instr_Rl_C_NoCarry);
    RUN_TEST(Instr_Rl_C_Zero);
    RUN_TEST(Instr_Bit_7_H_0);
    RUN_TEST(Instr_Bit_7_H_1);
    RUN_TEST(Instr_Bit_7_H_FromBootrom);
    RUN_TEST(Instr_JrNzS8_FromBootrom);
    RUN_TEST(Instr_Bit_7_H_FromBootrom_Break);
    RUN_TEST(Instr_JpA16);
    RUN_TEST(Instr_Di);
    RUN_TEST(Instr_Ei);
    RUN_TEST(Instr_Reti);
    RUN_TEST(Instr_LdHlD8);
    RUN_TEST(Instr_Cpl);
    RUN_TEST(Instr_AndD8);
    RUN_TEST(Instr_AndD8_0);
    RUN_TEST(Instr_Swap_A);
    RUN_TEST(Instr_Swap_A_0);
    RUN_TEST(Instr_Swap_HL);
    RUN_TEST(Instr_Rst5);
    RUN_TEST(Instr_Adc_B);
    RUN_TEST(Instr_Adc_HL);
    RUN_TEST(Instr_AdcD8);
    RUN_TEST(Instr_Rlca);
    RUN_TEST(Instr_JpHl);
    RUN_TEST(Instr_Res_B);
    RUN_TEST(Instr_Res_HL);
    RUN_TEST(Instr_IncHlAddr);
    RUN_TEST(Instr_JpFlagA16_Jump);
    RUN_TEST(Instr_JpFlagA16_NoJump);
    RUN_TEST(Instr_JpNFlagA16_Jump);
    RUN_TEST(Instr_JpNFlagA16_NoJump);
    RUN_TEST(Instr_DecHlAddr);
    RUN_TEST(Instr_Daa_0);
    RUN_TEST(Instr_Daa_1);
    RUN_TEST(Instr_Daa_10);
    RUN_TEST(Instr_Daa_12);
}
