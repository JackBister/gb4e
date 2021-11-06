#include "Instruction.hh"

#include <array>

#include "GbCpuState.hh"
#include "InstructionAppliers.hh"

namespace gb4e
{
InstructionApplier APPLIER_NOP = [](GbCpuState const *, MemoryState const *) { return InstructionResult(); };

Instruction const INSTR_INVALID(0xD3, 0x00, "INVALID", 1, 0, APPLIER_NOP);

Instruction const INSTR_00(0x00, 0x0000, "NOP", 1, 1, APPLIER_NOP);
Instruction const INSTR_01(0x01, 0x0100, "LD BC, d16", 3, 3, LdD16<RegisterName::BC>);
Instruction const INSTR_02(0x02, 0x0200, "LD (BC), A", 1, 2, LdMemViaReg<RegisterName::BC, RegisterName::A>);
Instruction const INSTR_03(0x03, 0x0300, "INC BC", 1, 2, Inc<RegisterName::BC>);
Instruction const INSTR_04(0x04, 0x0400, "INC B", 1, 1, Inc<RegisterName::B>);
Instruction const INSTR_05(0x05, 0x0500, "DEC B", 1, 1, Dec<RegisterName::B>);
Instruction const INSTR_06(0x06, 0x0600, "LD B, d8", 2, 2, LdD8<RegisterName::B>);
Instruction const INSTR_07(0x07, 0x0700, "RLCA", 1, 1, Rlca);
// 08
Instruction const INSTR_09(0x09, 0x0900, "ADD HL, BC", 1, 2, Add<RegisterName::HL, RegisterName::BC>);
Instruction const INSTR_0A(0x0A, 0x0A00, "LD A, (BC)", 1, 2, LdFromAddrReg<RegisterName::A, RegisterName::DE>);
Instruction const INSTR_0B(0x0B, 0x0B00, "DEC BC", 1, 2, Dec<RegisterName::BC>);
Instruction const INSTR_0C(0x0C, 0x0C00, "INC C", 1, 1, Inc<RegisterName::C>);
Instruction const INSTR_0D(0x0D, 0x0D00, "DEC C", 1, 1, Dec<RegisterName::C>);
Instruction const INSTR_0E(0x0E, 0x0E00, "LD C, d8", 2, 2, LdD8<RegisterName::C>);

// 10
Instruction const INSTR_11(0x11, 0x1100, "LD DE, d16", 3, 3, LdD16<RegisterName::DE>);
Instruction const INSTR_12(0x12, 0x1200, "LD (DE), A", 1, 2, LdMemViaReg<RegisterName::DE, RegisterName::A>);
Instruction const INSTR_13(0x13, 0x1300, "INC DE", 1, 2, Inc<RegisterName::DE>);
Instruction const INSTR_14(0x14, 0x1400, "INC D", 1, 1, Inc<RegisterName::D>);
Instruction const INSTR_15(0x15, 0x1500, "DEC D", 1, 1, Dec<RegisterName::D>);
Instruction const INSTR_16(0x16, 0x1600, "LD D, d8", 2, 2, LdD8<RegisterName::D>);
Instruction const INSTR_17(0x17, 0x1700, "RLA", 1, 1, Rla);
Instruction const INSTR_18(0x18, 0x1800, "JR s8", 2, 3, JrS8);
Instruction const INSTR_19(0x19, 0x1900, "ADD HL, DE", 1, 2, Add<RegisterName::HL, RegisterName::DE>);
Instruction const INSTR_1A(0x1A, 0x1A00, "LD A, (DE)", 1, 2, LdFromAddrReg<RegisterName::A, RegisterName::DE>);
Instruction const INSTR_1B(0x1B, 0x1B00, "DEC DE", 1, 2, Dec<RegisterName::DE>);
Instruction const INSTR_1C(0x1C, 0x1C00, "INC E", 1, 1, Inc<RegisterName::E>);
Instruction const INSTR_1D(0x1D, 0x1D00, "DEC E", 1, 1, Dec<RegisterName::E>);
Instruction const INSTR_1E(0x1E, 0x1E00, "LD E, d8", 2, 2, LdD8<RegisterName::E>);
// 1F

Instruction const INSTR_20(0x20, 0x2000, "JR NZ, s8", 2, 2, JrNFlagS8<0b10000000>); // TODO: Conditional 3 cycles
Instruction const INSTR_21(0x21, 0x2100, "LD HL, d16", 3, 3, LdD16<RegisterName::HL>);
Instruction const INSTR_22(0x22, 0x2200, "LD (HL+), A", 1, 2, LdHlIncDecA<1>);
Instruction const INSTR_23(0x23, 0x2300, "INC HL", 1, 2, Inc<RegisterName::HL>);
Instruction const INSTR_24(0x24, 0x2400, "INC H", 1, 1, Inc<RegisterName::H>);
Instruction const INSTR_25(0x25, 0x2500, "DEC H", 1, 1, Dec<RegisterName::H>);
Instruction const INSTR_26(0x26, 0x2600, "LD H, d8", 2, 2, LdD8<RegisterName::H>);
Instruction const INSTR_27(0x27, 0x2700, "DAA", 1, 1, Daa);
Instruction const INSTR_28(0x28, 0x2800, "JR Z, s8", 2, 2, JrFlagS8<0b10000000>); // TODO: Conditional 3 cycles
Instruction const INSTR_29(0x29, 0x2900, "ADD HL, HL", 1, 2, Add<RegisterName::HL, RegisterName::HL>);
Instruction const INSTR_2A(0x2A, 0x2A00, "LD A, (HL+)", 1, 2, LdFromAddrReg<RegisterName::A, RegisterName::HL, 1>);
Instruction const INSTR_2B(0x2B, 0x2B00, "DEC HL", 1, 2, Dec<RegisterName::HL>);
Instruction const INSTR_2C(0x2C, 0x2C00, "INC L", 1, 1, Inc<RegisterName::L>);
Instruction const INSTR_2D(0x2D, 0x2D00, "DEC L", 1, 1, Dec<RegisterName::L>);
Instruction const INSTR_2E(0x2E, 0x2E00, "LD L, d8", 2, 2, LdD8<RegisterName::L>);
Instruction const INSTR_2F(0x2F, 0x2F00, "CPL", 1, 1, Cpl);

Instruction const INSTR_30(0x30, 0x3000, "JR NC, s8", 2, 2, JrNFlagS8<0b00010000>); // TODO: Conditional 3 cycles
Instruction const INSTR_31(0x31, 0x3100, "LD SP, d16", 3, 3, LdD16<RegisterName::SP>);
Instruction const INSTR_32(0x32, 0x3200, "LD (HL-), A", 1, 2, LdHlIncDecA<-1>);
Instruction const INSTR_33(0x33, 0x3300, "INC SP", 1, 2, Inc<RegisterName::SP>);
Instruction const INSTR_34(0x34, 0x3400, "INC (HL)", 1, 3, IncHlAddr);
Instruction const INSTR_35(0x35, 0x3500, "DEC (HL)", 1, 3, DecHlAddr);
Instruction const INSTR_36(0x36, 0x3600, "LD (HL), d8", 2, 3, LdHlD8);
// 37
Instruction const INSTR_38(0x38, 0x3800, "JR C, s8", 2, 2, JrFlagS8<0b0001>);
Instruction const INSTR_39(0x39, 0x3900, "ADD HL, SP", 1, 2, Add<RegisterName::HL, RegisterName::SP>);
Instruction const INSTR_3A(0x3A, 0x3A00, "LD A, (HL-)", 1, 2, LdFromAddrReg<RegisterName::A, RegisterName::HL, -1>);
Instruction const INSTR_3B(0x3B, 0x3B00, "DEC SP", 1, 2, Dec<RegisterName::SP>);
Instruction const INSTR_3C(0x3C, 0x3C00, "INC A", 1, 1, Inc<RegisterName::A>);
Instruction const INSTR_3D(0x3D, 0x3D00, "DEC A", 1, 1, Dec<RegisterName::A>);
Instruction const INSTR_3E(0x3E, 0x3E00, "LD A, d8", 2, 2, LdD8<RegisterName::A>);
// 3F

Instruction const INSTR_40(0x40, 0x4000, "LD B, B", 1, 1, Ld<RegisterName::B, RegisterName::B>);
Instruction const INSTR_41(0x41, 0x4100, "LD B, C", 1, 1, Ld<RegisterName::B, RegisterName::C>);
Instruction const INSTR_42(0x42, 0x4200, "LD B, D", 1, 1, Ld<RegisterName::B, RegisterName::D>);
Instruction const INSTR_43(0x43, 0x4300, "LD B, E", 1, 1, Ld<RegisterName::B, RegisterName::E>);
Instruction const INSTR_44(0x44, 0x4400, "LD B, H", 1, 1, Ld<RegisterName::B, RegisterName::H>);
Instruction const INSTR_45(0x45, 0x4500, "LD B, L", 1, 1, Ld<RegisterName::B, RegisterName::L>);
Instruction const INSTR_46(0x46, 0x4600, "LD B, (HL)", 1, 2, LdFromAddrReg<RegisterName::B, RegisterName::HL>);
Instruction const INSTR_47(0x47, 0x4700, "LD B, A", 1, 1, Ld<RegisterName::B, RegisterName::A>);
Instruction const INSTR_48(0x48, 0x4800, "LD C, B", 1, 1, Ld<RegisterName::C, RegisterName::B>);
Instruction const INSTR_49(0x49, 0x4900, "LD C, C", 1, 1, Ld<RegisterName::C, RegisterName::C>);
Instruction const INSTR_4A(0x4A, 0x4A00, "LD C, D", 1, 1, Ld<RegisterName::C, RegisterName::D>);
Instruction const INSTR_4B(0x4B, 0x4B00, "LD C, E", 1, 1, Ld<RegisterName::C, RegisterName::E>);
Instruction const INSTR_4C(0x4C, 0x4C00, "LD C, H", 1, 1, Ld<RegisterName::C, RegisterName::H>);
Instruction const INSTR_4D(0x4D, 0x4D00, "LD C, L", 1, 1, Ld<RegisterName::C, RegisterName::L>);
Instruction const INSTR_4E(0x4E, 0x4E00, "LD C, (HL)", 1, 2, LdFromAddrReg<RegisterName::B, RegisterName::HL>);
Instruction const INSTR_4F(0x4F, 0x4F00, "LD C, A", 1, 1, Ld<RegisterName::C, RegisterName::A>);

Instruction const INSTR_50(0x50, 0x5000, "LD D, B", 1, 1, Ld<RegisterName::D, RegisterName::B>);
Instruction const INSTR_51(0x51, 0x5100, "LD D, C", 1, 1, Ld<RegisterName::D, RegisterName::C>);
Instruction const INSTR_52(0x52, 0x5200, "LD D, D", 1, 1, Ld<RegisterName::D, RegisterName::D>);
Instruction const INSTR_53(0x53, 0x5300, "LD D, E", 1, 1, Ld<RegisterName::D, RegisterName::E>);
Instruction const INSTR_54(0x54, 0x5400, "LD D, H", 1, 1, Ld<RegisterName::D, RegisterName::H>);
Instruction const INSTR_55(0x55, 0x5500, "LD D, L", 1, 1, Ld<RegisterName::D, RegisterName::L>);
Instruction const INSTR_56(0x56, 0x5600, "LD D, (HL)", 1, 2, LdFromAddrReg<RegisterName::D, RegisterName::HL>);
Instruction const INSTR_57(0x57, 0x5700, "LD D, A", 1, 1, Ld<RegisterName::D, RegisterName::A>);
Instruction const INSTR_58(0x58, 0x5800, "LD E, B", 1, 1, Ld<RegisterName::E, RegisterName::B>);
Instruction const INSTR_59(0x59, 0x5900, "LD E, C", 1, 1, Ld<RegisterName::E, RegisterName::C>);
Instruction const INSTR_5A(0x5A, 0x5A00, "LD E, D", 1, 1, Ld<RegisterName::E, RegisterName::D>);
Instruction const INSTR_5B(0x5B, 0x5B00, "LD E, E", 1, 1, Ld<RegisterName::E, RegisterName::E>);
Instruction const INSTR_5C(0x5C, 0x5C00, "LD E, H", 1, 1, Ld<RegisterName::E, RegisterName::H>);
Instruction const INSTR_5D(0x5D, 0x5D00, "LD E, L", 1, 1, Ld<RegisterName::E, RegisterName::L>);
Instruction const INSTR_5E(0x5E, 0x5E00, "LD E, (HL)", 1, 2, LdFromAddrReg<RegisterName::E, RegisterName::HL>);
Instruction const INSTR_5F(0x5F, 0x5F00, "LD E, A", 1, 1, Ld<RegisterName::E, RegisterName::A>);

Instruction const INSTR_60(0x60, 0x6000, "LD H, B", 1, 1, Ld<RegisterName::H, RegisterName::B>);
Instruction const INSTR_61(0x61, 0x6100, "LD H, C", 1, 1, Ld<RegisterName::H, RegisterName::C>);
Instruction const INSTR_62(0x62, 0x6200, "LD H, D", 1, 1, Ld<RegisterName::H, RegisterName::D>);
Instruction const INSTR_63(0x63, 0x6300, "LD H, E", 1, 1, Ld<RegisterName::H, RegisterName::E>);
Instruction const INSTR_64(0x64, 0x6400, "LD H, H", 1, 1, Ld<RegisterName::H, RegisterName::H>);
Instruction const INSTR_65(0x65, 0x6500, "LD H, L", 1, 1, Ld<RegisterName::H, RegisterName::L>);
Instruction const INSTR_66(0x66, 0x6600, "LD H, (HL)", 1, 2, LdFromAddrReg<RegisterName::H, RegisterName::HL>);
Instruction const INSTR_67(0x67, 0x6700, "LD H, A", 1, 1, Ld<RegisterName::H, RegisterName::A>);
Instruction const INSTR_68(0x68, 0x6800, "LD L, B", 1, 1, Ld<RegisterName::L, RegisterName::B>);
Instruction const INSTR_69(0x69, 0x6900, "LD L, C", 1, 1, Ld<RegisterName::L, RegisterName::C>);
Instruction const INSTR_6A(0x6A, 0x6A00, "LD L, D", 1, 1, Ld<RegisterName::L, RegisterName::D>);
Instruction const INSTR_6B(0x6B, 0x6B00, "LD L, E", 1, 1, Ld<RegisterName::L, RegisterName::E>);
Instruction const INSTR_6C(0x6C, 0x6C00, "LD L, H", 1, 1, Ld<RegisterName::L, RegisterName::H>);
Instruction const INSTR_6D(0x6D, 0x6D00, "LD L, L", 1, 1, Ld<RegisterName::L, RegisterName::L>);
Instruction const INSTR_6E(0x6E, 0x6E00, "LD L, (HL)", 1, 2, LdFromAddrReg<RegisterName::L, RegisterName::HL>);
Instruction const INSTR_6F(0x6F, 0x6F00, "LD L, A", 1, 1, Ld<RegisterName::L, RegisterName::A>);

Instruction const INSTR_70(0x70, 0x7000, "LD (HL), B", 1, 2, LdMemViaReg<RegisterName::HL, RegisterName::B>);
Instruction const INSTR_71(0x71, 0x7100, "LD (HL), C", 1, 2, LdMemViaReg<RegisterName::HL, RegisterName::C>);
Instruction const INSTR_72(0x72, 0x7200, "LD (HL), D", 1, 2, LdMemViaReg<RegisterName::HL, RegisterName::D>);
Instruction const INSTR_73(0x73, 0x7300, "LD (HL), E", 1, 2, LdMemViaReg<RegisterName::HL, RegisterName::E>);
Instruction const INSTR_74(0x74, 0x7400, "LD (HL), H", 1, 2, LdMemViaReg<RegisterName::HL, RegisterName::H>);
Instruction const INSTR_75(0x75, 0x7500, "LD (HL), L", 1, 2, LdMemViaReg<RegisterName::HL, RegisterName::L>);
// 76
Instruction const INSTR_77(0x77, 0x7700, "LD (HL), A", 1, 2, LdMemViaReg<RegisterName::HL, RegisterName::A>);
Instruction const INSTR_78(0x78, 0x7800, "LD A, B", 1, 1, Ld<RegisterName::A, RegisterName::B>);
Instruction const INSTR_79(0x79, 0x7900, "LD A, C", 1, 1, Ld<RegisterName::A, RegisterName::C>);
Instruction const INSTR_7A(0x7A, 0x7A00, "LD A, D", 1, 1, Ld<RegisterName::A, RegisterName::D>);
Instruction const INSTR_7B(0x7B, 0x7B00, "LD A, E", 1, 1, Ld<RegisterName::A, RegisterName::E>);
Instruction const INSTR_7C(0x7C, 0x7C00, "LD A, H", 1, 1, Ld<RegisterName::A, RegisterName::H>);
Instruction const INSTR_7D(0x7D, 0x7D00, "LD A, L", 1, 1, Ld<RegisterName::A, RegisterName::L>);
Instruction const INSTR_7E(0x7E, 0x7E00, "LD A, (HL)", 1, 2, LdFromAddrReg<RegisterName::A, RegisterName::HL>);
// 7E
Instruction const INSTR_7F(0x7F, 0x7F00, "LD A, A", 1, 1, Ld<RegisterName::A, RegisterName::A>);

Instruction const INSTR_80(0x80, 0x8000, "ADD A, B", 1, 1, Add<RegisterName::A, RegisterName::B>);
Instruction const INSTR_81(0x81, 0x8100, "ADD A, C", 1, 1, Add<RegisterName::A, RegisterName::C>);
Instruction const INSTR_82(0x82, 0x8200, "ADD A, D", 1, 1, Add<RegisterName::A, RegisterName::D>);
Instruction const INSTR_83(0x83, 0x8300, "ADD A, E", 1, 1, Add<RegisterName::A, RegisterName::E>);
Instruction const INSTR_84(0x84, 0x8400, "ADD A, H", 1, 1, Add<RegisterName::A, RegisterName::H>);
Instruction const INSTR_85(0x85, 0x8500, "ADD A, L", 1, 1, Add<RegisterName::A, RegisterName::L>);
Instruction const INSTR_86(0x86, 0x8600, "ADD A, (HL)", 1, 2, AddFromAddrReg<RegisterName::A, RegisterName::HL>);
Instruction const INSTR_87(0x87, 0x8700, "ADD A, A", 1, 1, Add<RegisterName::A, RegisterName::A>);

Instruction const INSTR_88(0x88, 0x8800, "ADC B", 1, 2, Adc<RegisterName::B>);
Instruction const INSTR_89(0x89, 0x8900, "ADC C", 1, 2, Adc<RegisterName::C>);
Instruction const INSTR_8A(0x8A, 0x8A00, "ADC D", 1, 2, Adc<RegisterName::D>);
Instruction const INSTR_8B(0x8B, 0x8B00, "ADC E", 1, 2, Adc<RegisterName::E>);
Instruction const INSTR_8C(0x8C, 0x8C00, "ADC H", 1, 2, Adc<RegisterName::H>);
Instruction const INSTR_8D(0x8D, 0x8D00, "ADC L", 1, 2, Adc<RegisterName::L>);
Instruction const INSTR_8E(0x8E, 0x8E00, "ADC (HL)", 1, 2, Adc<RegisterName::HL>);
Instruction const INSTR_8F(0x8F, 0x8F00, "ADC A", 1, 2, Adc<RegisterName::A>);

Instruction const INSTR_90(0x90, 0x9000, "SUB B", 1, 1, Sub<RegisterName::B>);
Instruction const INSTR_91(0x91, 0x9100, "SUB C", 1, 1, Sub<RegisterName::C>);
Instruction const INSTR_92(0x92, 0x9200, "SUB D", 1, 1, Sub<RegisterName::D>);
Instruction const INSTR_93(0x93, 0x9300, "SUB E", 1, 1, Sub<RegisterName::E>);
Instruction const INSTR_94(0x94, 0x9400, "SUB H", 1, 1, Sub<RegisterName::H>);
Instruction const INSTR_95(0x95, 0x9500, "SUB L", 1, 1, Sub<RegisterName::L>);
// 96
Instruction const INSTR_97(0x97, 0x9700, "SUB A", 1, 1, Sub<RegisterName::A>);
// 98-9F

Instruction const INSTR_A0(0xA0, 0xA000, "AND B", 1, 1, And<RegisterName::B>);
Instruction const INSTR_A1(0xA1, 0xA100, "AND C", 1, 1, And<RegisterName::C>);
Instruction const INSTR_A2(0xA2, 0xA200, "AND D", 1, 1, And<RegisterName::D>);
Instruction const INSTR_A3(0xA3, 0xA300, "AND E", 1, 1, And<RegisterName::E>);
Instruction const INSTR_A4(0xA4, 0xA400, "AND H", 1, 1, And<RegisterName::H>);
Instruction const INSTR_A5(0xA5, 0xA500, "AND L", 1, 1, And<RegisterName::L>);
// A6
Instruction const INSTR_A7(0xA7, 0xA700, "AND A", 1, 1, And<RegisterName::A>);

Instruction const INSTR_A8(0xA8, 0xA800, "XOR B", 1, 1, Xor<RegisterName::B>);
Instruction const INSTR_A9(0xA9, 0xA900, "XOR C", 1, 1, Xor<RegisterName::C>);
Instruction const INSTR_AA(0xAA, 0xAA00, "XOR D", 1, 1, Xor<RegisterName::D>);
Instruction const INSTR_AB(0xAB, 0xAB00, "XOR E", 1, 1, Xor<RegisterName::E>);
Instruction const INSTR_AC(0xAC, 0xAC00, "XOR H", 1, 1, Xor<RegisterName::H>);
Instruction const INSTR_AD(0xAD, 0xAD00, "XOR L", 1, 1, Xor<RegisterName::L>);
Instruction const INSTR_AE(0xAE, 0xAE00, "XOR (HL)", 1, 2, Xor<RegisterName::HL>);
Instruction const INSTR_AF(0xAF, 0xAF00, "XOR A", 1, 1, Xor<RegisterName::A>);

Instruction const INSTR_B0(0xB0, 0xB000, "OR B", 1, 1, Or<RegisterName::B>);
Instruction const INSTR_B1(0xB1, 0xB100, "OR C", 1, 1, Or<RegisterName::C>);
Instruction const INSTR_B2(0xB2, 0xB200, "OR D", 1, 1, Or<RegisterName::D>);
Instruction const INSTR_B3(0xB3, 0xB300, "OR E", 1, 1, Or<RegisterName::E>);
Instruction const INSTR_B4(0xB4, 0xB400, "OR H", 1, 1, Or<RegisterName::H>);
Instruction const INSTR_B5(0xB5, 0xB500, "OR L", 1, 1, Or<RegisterName::L>);
// B6
Instruction const INSTR_B7(0xB7, 0xB700, "OR A", 1, 1, Or<RegisterName::A>);
// B8-BD
Instruction const INSTR_BE(0xBE, 0xBE00, "CP (HL)", 1, 2, CpFromMem<RegisterName::HL>);
// BF

Instruction const INSTR_C0(0xC0, 0xC000, "RET NZ", 1, 2, RetNFlag<0b10000000>); // TODO: Conditional 5 cycles
Instruction const INSTR_C1(0xC1, 0xC100, "POP BC", 1, 3, Pop<RegisterName::BC>);
Instruction const INSTR_C2(0xC2, 0xC200, "JP NZ, a16", 3, 3, JpNFlagA16<0b10000000>);
Instruction const INSTR_C3(0xC3, 0xC300, "JP a16", 3, 4, JpA16);
// C4
Instruction const INSTR_C5(0xC5, 0xC500, "PUSH BC", 1, 4, Push<RegisterName::BC>);
Instruction const INSTR_C6(0xC6, 0xC600, "ADD A, d8", 2, 2, AddD8<RegisterName::A>);
Instruction const INSTR_C7(0xC7, 0xC700, "RST 0", 1, 4, Rst<0>);
Instruction const INSTR_C8(0xC8, 0xC800, "RET Z", 1, 2, RetFlag<0b10000000>);
Instruction const INSTR_C9(0xC9, 0xC900, "RET", 1, 4, Ret);
Instruction const INSTR_CA(0xCA, 0xCA00, "JP Z, a16", 3, 3, JpFlagA16<0b10000000>);
Instruction const INSTR_CB(0xCB, 0xCB00, "INVALID CB", 1, 1, APPLIER_NOP);
// CC
Instruction const INSTR_CD(0xCD, 0xCD00, "CALL a16", 3, 6, CallA16);
Instruction const INSTR_CE(0xCE, 0xCE00, "ADC d8", 2, 2, AdcD8);
Instruction const INSTR_CF(0xCF, 0xCF00, "RST 1", 1, 4, Rst<1>);

Instruction const INSTR_D0(0xD0, 0xD000, "RET NC", 1, 2, RetNFlag<0b00010000>);
Instruction const INSTR_D1(0xD1, 0xD100, "POP DE", 1, 3, Pop<RegisterName::DE>);
Instruction const INSTR_D2(0xD2, 0xD200, "JP NC, a16", 3, 3, JpNFlagA16<0b00010000>);
Instruction const INSTR_D3(0xD3, 0xD300, "INVALID D3", 1, 1, APPLIER_NOP);
// D4
Instruction const INSTR_D5(0xD5, 0xD500, "PUSH DE", 1, 4, Push<RegisterName::DE>);
// D6
Instruction const INSTR_D7(0xD7, 0xD700, "RST 2", 1, 4, Rst<2>);
Instruction const INSTR_D8(0xD8, 0xD800, "RET C", 1, 2, RetFlag<0b00010000>);
Instruction const INSTR_D9(0xD9, 0xD900, "RETI", 1, 4, Reti);
Instruction const INSTR_DA(0xDA, 0xDA00, "JP C, a16", 3, 3, JpFlagA16<0b00010000>);
Instruction const INSTR_DB(0xDB, 0xDB00, "INVALID DB", 1, 1, APPLIER_NOP);
// DC
Instruction const INSTR_DD(0xDD, 0xDD00, "INVALID DD", 1, 1, APPLIER_NOP);
// DE
Instruction const INSTR_DF(0xDF, 0xDF00, "RST 3", 1, 4, Rst<3>);

Instruction const INSTR_E0(0xE0, 0xE000, "LD (a8), A", 2, 3, LdA8);
Instruction const INSTR_E1(0xE1, 0xE100, "POP HL", 1, 3, Pop<RegisterName::HL>);
Instruction const INSTR_E2(0xE2, 0xE200, "LD (C), A", 1, 2, LdMemViaReg<RegisterName::C, RegisterName::A>);
Instruction const INSTR_E3(0xE3, 0xE300, "INVALID E3", 1, 1, APPLIER_NOP);
Instruction const INSTR_E4(0xE4, 0xE400, "INVALID E4", 1, 1, APPLIER_NOP);
Instruction const INSTR_E5(0xE5, 0xE500, "PUSH HL", 1, 4, Push<RegisterName::HL>);
Instruction const INSTR_E6(0xE6, 0xE600, "AND A, d8", 2, 2, AndD8);
Instruction const INSTR_E7(0xE7, 0xE700, "RST 4", 1, 4, Rst<4>);
// E8
Instruction const INSTR_E9(0xE9, 0xE900, "JP HL", 1, 1, JpHl);
Instruction const INSTR_EA(0xEA, 0xEA00, "LD (a16), A", 3, 4, LdA16<RegisterName::A>);
Instruction const INSTR_EB(0xEB, 0xEB00, "INVALID EB", 1, 1, APPLIER_NOP);
Instruction const INSTR_EC(0xEC, 0xEC00, "INVALID EC", 1, 1, APPLIER_NOP);
Instruction const INSTR_ED(0xED, 0xED00, "INVALID ED", 1, 1, APPLIER_NOP);
// EE
Instruction const INSTR_EF(0xEF, 0xEF00, "RST 5", 1, 4, Rst<5>);

Instruction const INSTR_F0(0xF0, 0xF000, "LD A, (a8)", 2, 3, LdFromA8<RegisterName::A>);
Instruction const INSTR_F1(0xF1, 0xF100, "POP AF", 1, 3, Pop<RegisterName::AF>);
// F2
Instruction const INSTR_F3(0xF3, 0xF300, "DI", 1, 1, ToggleInterrupts<false>);
Instruction const INSTR_F4(0xF4, 0xF400, "INVALID F4", 1, 1, APPLIER_NOP);
Instruction const INSTR_F5(0xF5, 0xF500, "PUSH AF", 1, 4, Push<RegisterName::AF>);
// F6
Instruction const INSTR_F7(0xF7, 0xF700, "RST 6", 1, 4, Rst<6>);
// F8-F9
Instruction const INSTR_FA(0xFA, 0xFA00, "LD A, (a16)", 3, 4, LdFromA16<RegisterName::A>);
Instruction const INSTR_FB(0xFB, 0xFB00, "EI", 1, 1, ToggleInterrupts<true>);
Instruction const INSTR_FC(0xFC, 0xFC00, "INVALID FC", 1, 1, APPLIER_NOP);
Instruction const INSTR_FD(0xFD, 0xFD00, "INVALID FD", 1, 1, APPLIER_NOP);
Instruction const INSTR_FE(0xFE, 0xFE00, "CP d8", 2, 2, CpD8);
Instruction const INSTR_FF(0xFF, 0xFF00, "RST 7", 1, 4, Rst<7>);

Instruction const INSTR_CB10(0xCB, 0xCB10, "RL B", 2, 2, Rl<RegisterName::B>);
Instruction const INSTR_CB11(0xCB, 0xCB11, "RL C", 2, 2, Rl<RegisterName::C>);
Instruction const INSTR_CB12(0xCB, 0xCB12, "RL D", 2, 2, Rl<RegisterName::D>);
Instruction const INSTR_CB13(0xCB, 0xCB13, "RL E", 2, 2, Rl<RegisterName::E>);
Instruction const INSTR_CB14(0xCB, 0xCB14, "RL H", 2, 2, Rl<RegisterName::H>);
Instruction const INSTR_CB15(0xCB, 0xCB15, "RL L", 2, 2, Rl<RegisterName::L>);
// CB16
Instruction const INSTR_CB17(0xCB, 0xCB17, "RL A", 2, 2, Rl<RegisterName::A>);

Instruction const INSTR_CB20(0xCB, 0xCB20, "SLA B", 2, 2, Sla<RegisterName::B>);
Instruction const INSTR_CB21(0xCB, 0xCB21, "SLA C", 2, 2, Sla<RegisterName::C>);
Instruction const INSTR_CB22(0xCB, 0xCB22, "SLA D", 2, 2, Sla<RegisterName::D>);
Instruction const INSTR_CB23(0xCB, 0xCB23, "SLA E", 2, 2, Sla<RegisterName::E>);
Instruction const INSTR_CB24(0xCB, 0xCB24, "SLA H", 2, 2, Sla<RegisterName::H>);
Instruction const INSTR_CB25(0xCB, 0xCB25, "SLA L", 2, 2, Sla<RegisterName::L>);
// CB26
Instruction const INSTR_CB27(0xCB, 0xCB27, "SLA A", 2, 2, Sla<RegisterName::A>);

Instruction const INSTR_CB30(0xCB, 0xCB30, "SWAP B", 2, 2, Swap<RegisterName::B>);
Instruction const INSTR_CB31(0xCB, 0xCB31, "SWAP C", 2, 2, Swap<RegisterName::C>);
Instruction const INSTR_CB32(0xCB, 0xCB32, "SWAP D", 2, 2, Swap<RegisterName::D>);
Instruction const INSTR_CB33(0xCB, 0xCB33, "SWAP E", 2, 2, Swap<RegisterName::E>);
Instruction const INSTR_CB34(0xCB, 0xCB34, "SWAP H", 2, 2, Swap<RegisterName::H>);
Instruction const INSTR_CB35(0xCB, 0xCB35, "SWAP L", 2, 2, Swap<RegisterName::L>);
Instruction const INSTR_CB36(0xCB, 0xCB36, "SWAP (HL)", 2, 4, Swap<RegisterName::HL>);
Instruction const INSTR_CB37(0xCB, 0xCB37, "SWAP A", 2, 2, Swap<RegisterName::A>);

Instruction const INSTR_CB40(0xCB, 0xCB40, "BIT 0, B", 2, 2, Bit<0, RegisterName::B>);
Instruction const INSTR_CB41(0xCB, 0xCB41, "BIT 0, C", 2, 2, Bit<0, RegisterName::C>);
Instruction const INSTR_CB42(0xCB, 0xCB42, "BIT 0, D", 2, 2, Bit<0, RegisterName::D>);
Instruction const INSTR_CB43(0xCB, 0xCB43, "BIT 0, E", 2, 2, Bit<0, RegisterName::E>);
Instruction const INSTR_CB44(0xCB, 0xCB44, "BIT 0, H", 2, 2, Bit<0, RegisterName::H>);
Instruction const INSTR_CB45(0xCB, 0xCB45, "BIT 0, L", 2, 2, Bit<0, RegisterName::L>);
// CB46
Instruction const INSTR_CB47(0xCB, 0xCB47, "BIT 0, A", 2, 2, Bit<0, RegisterName::A>);
Instruction const INSTR_CB48(0xCB, 0xCB48, "BIT 1, B", 2, 2, Bit<1, RegisterName::B>);
Instruction const INSTR_CB49(0xCB, 0xCB49, "BIT 1, C", 2, 2, Bit<1, RegisterName::C>);
Instruction const INSTR_CB4A(0xCB, 0xCB4A, "BIT 1, D", 2, 2, Bit<1, RegisterName::D>);
Instruction const INSTR_CB4B(0xCB, 0xCB4B, "BIT 1, E", 2, 2, Bit<1, RegisterName::E>);
Instruction const INSTR_CB4C(0xCB, 0xCB4C, "BIT 1, H", 2, 2, Bit<1, RegisterName::H>);
Instruction const INSTR_CB4D(0xCB, 0xCB4D, "BIT 1, L", 2, 2, Bit<1, RegisterName::L>);
// CB4E
Instruction const INSTR_CB4F(0xCB, 0xCB4F, "BIT 1, A", 2, 2, Bit<1, RegisterName::A>);

Instruction const INSTR_CB50(0xCB, 0xCB50, "BIT 2, B", 2, 2, Bit<2, RegisterName::B>);
Instruction const INSTR_CB51(0xCB, 0xCB51, "BIT 2, C", 2, 2, Bit<2, RegisterName::C>);
Instruction const INSTR_CB52(0xCB, 0xCB52, "BIT 2, D", 2, 2, Bit<2, RegisterName::D>);
Instruction const INSTR_CB53(0xCB, 0xCB53, "BIT 2, E", 2, 2, Bit<2, RegisterName::E>);
Instruction const INSTR_CB54(0xCB, 0xCB54, "BIT 2, H", 2, 2, Bit<2, RegisterName::H>);
Instruction const INSTR_CB55(0xCB, 0xCB55, "BIT 2, L", 2, 2, Bit<2, RegisterName::L>);
// CB56
Instruction const INSTR_CB57(0xCB, 0xCB57, "BIT 2, A", 2, 2, Bit<2, RegisterName::A>);
Instruction const INSTR_CB58(0xCB, 0xCB58, "BIT 3, B", 2, 2, Bit<3, RegisterName::B>);
Instruction const INSTR_CB59(0xCB, 0xCB59, "BIT 3, C", 2, 2, Bit<3, RegisterName::C>);
Instruction const INSTR_CB5A(0xCB, 0xCB5A, "BIT 3, D", 2, 2, Bit<3, RegisterName::D>);
Instruction const INSTR_CB5B(0xCB, 0xCB5B, "BIT 3, E", 2, 2, Bit<3, RegisterName::E>);
Instruction const INSTR_CB5C(0xCB, 0xCB5C, "BIT 3, H", 2, 2, Bit<3, RegisterName::H>);
Instruction const INSTR_CB5D(0xCB, 0xCB5D, "BIT 3, L", 2, 2, Bit<3, RegisterName::L>);
// CB5E
Instruction const INSTR_CB5F(0xCB, 0xCB5F, "BIT 3, A", 2, 2, Bit<3, RegisterName::A>);

Instruction const INSTR_CB60(0xCB, 0xCB60, "BIT 4, B", 2, 2, Bit<4, RegisterName::B>);
Instruction const INSTR_CB61(0xCB, 0xCB61, "BIT 4, C", 2, 2, Bit<4, RegisterName::C>);
Instruction const INSTR_CB62(0xCB, 0xCB62, "BIT 4, D", 2, 2, Bit<4, RegisterName::D>);
Instruction const INSTR_CB63(0xCB, 0xCB63, "BIT 4, E", 2, 2, Bit<4, RegisterName::E>);
Instruction const INSTR_CB64(0xCB, 0xCB64, "BIT 4, H", 2, 2, Bit<4, RegisterName::H>);
Instruction const INSTR_CB65(0xCB, 0xCB65, "BIT 4, L", 2, 2, Bit<4, RegisterName::L>);
// CB66
Instruction const INSTR_CB67(0xCB, 0xCB67, "BIT 4, A", 2, 2, Bit<4, RegisterName::A>);
Instruction const INSTR_CB68(0xCB, 0xCB68, "BIT 5, B", 2, 2, Bit<5, RegisterName::B>);
Instruction const INSTR_CB69(0xCB, 0xCB69, "BIT 5, C", 2, 2, Bit<5, RegisterName::C>);
Instruction const INSTR_CB6A(0xCB, 0xCB6A, "BIT 5, D", 2, 2, Bit<5, RegisterName::D>);
Instruction const INSTR_CB6B(0xCB, 0xCB6B, "BIT 5, E", 2, 2, Bit<5, RegisterName::E>);
Instruction const INSTR_CB6C(0xCB, 0xCB6C, "BIT 5, H", 2, 2, Bit<5, RegisterName::H>);
Instruction const INSTR_CB6D(0xCB, 0xCB6D, "BIT 5, L", 2, 2, Bit<5, RegisterName::L>);
// CB6E
Instruction const INSTR_CB6F(0xCB, 0xCB6F, "BIT 5, A", 2, 2, Bit<5, RegisterName::A>);

Instruction const INSTR_CB70(0xCB, 0xCB70, "BIT 6, B", 2, 2, Bit<6, RegisterName::B>);
Instruction const INSTR_CB71(0xCB, 0xCB71, "BIT 6, C", 2, 2, Bit<6, RegisterName::C>);
Instruction const INSTR_CB72(0xCB, 0xCB72, "BIT 6, D", 2, 2, Bit<6, RegisterName::D>);
Instruction const INSTR_CB73(0xCB, 0xCB73, "BIT 6, E", 2, 2, Bit<6, RegisterName::E>);
Instruction const INSTR_CB74(0xCB, 0xCB74, "BIT 6, H", 2, 2, Bit<6, RegisterName::H>);
Instruction const INSTR_CB75(0xCB, 0xCB75, "BIT 6, L", 2, 2, Bit<6, RegisterName::L>);
// CB76
Instruction const INSTR_CB77(0xCB, 0xCB77, "BIT 6, A", 2, 2, Bit<6, RegisterName::A>);
Instruction const INSTR_CB78(0xCB, 0xCB78, "BIT 7, B", 2, 2, Bit<7, RegisterName::B>);
Instruction const INSTR_CB79(0xCB, 0xCB79, "BIT 7, C", 2, 2, Bit<7, RegisterName::C>);
Instruction const INSTR_CB7A(0xCB, 0xCB7A, "BIT 7, D", 2, 2, Bit<7, RegisterName::D>);
Instruction const INSTR_CB7B(0xCB, 0xCB7B, "BIT 7, E", 2, 2, Bit<7, RegisterName::E>);
Instruction const INSTR_CB7C(0xCB, 0xCB7C, "BIT 7, H", 2, 2, Bit<7, RegisterName::H>);
Instruction const INSTR_CB7D(0xCB, 0xCB7D, "BIT 7, L", 2, 2, Bit<7, RegisterName::L>);
// CB7E
Instruction const INSTR_CB7F(0xCB, 0xCB7F, "BIT 7, A", 2, 2, Bit<7, RegisterName::A>);

Instruction const INSTR_CB80(0xCB, 0xCB80, "RES 0, B", 2, 2, Res<0, RegisterName::B>);
Instruction const INSTR_CB81(0xCB, 0xCB81, "RES 0, C", 2, 2, Res<0, RegisterName::C>);
Instruction const INSTR_CB82(0xCB, 0xCB82, "RES 0, D", 2, 2, Res<0, RegisterName::D>);
Instruction const INSTR_CB83(0xCB, 0xCB83, "RES 0, E", 2, 2, Res<0, RegisterName::E>);
Instruction const INSTR_CB84(0xCB, 0xCB84, "RES 0, H", 2, 2, Res<0, RegisterName::H>);
Instruction const INSTR_CB85(0xCB, 0xCB85, "RES 0, L", 2, 2, Res<0, RegisterName::L>);
Instruction const INSTR_CB86(0xCB, 0xCB86, "RES 0, (HL)", 2, 4, Res<0, RegisterName::HL>);
Instruction const INSTR_CB87(0xCB, 0xCB87, "RES 0, A", 2, 2, Res<0, RegisterName::A>);

Instruction const INSTR_CB88(0xCB, 0xCB88, "RES 1, B", 2, 2, Res<1, RegisterName::B>);
Instruction const INSTR_CB89(0xCB, 0xCB89, "RES 1, C", 2, 2, Res<1, RegisterName::C>);
Instruction const INSTR_CB8A(0xCB, 0xCB8A, "RES 1, D", 2, 2, Res<1, RegisterName::D>);
Instruction const INSTR_CB8B(0xCB, 0xCB8B, "RES 1, E", 2, 2, Res<1, RegisterName::E>);
Instruction const INSTR_CB8C(0xCB, 0xCB8C, "RES 1, H", 2, 2, Res<1, RegisterName::H>);
Instruction const INSTR_CB8D(0xCB, 0xCB8D, "RES 1, L", 2, 2, Res<1, RegisterName::L>);
Instruction const INSTR_CB8E(0xCB, 0xCB8E, "RES 1, (HL)", 2, 4, Res<1, RegisterName::HL>);
Instruction const INSTR_CB8F(0xCB, 0xCB8F, "RES 1, A", 2, 2, Res<1, RegisterName::A>);

Instruction const INSTR_CB90(0xCB, 0xCB90, "RES 2, B", 2, 2, Res<2, RegisterName::B>);
Instruction const INSTR_CB91(0xCB, 0xCB91, "RES 2, C", 2, 2, Res<2, RegisterName::C>);
Instruction const INSTR_CB92(0xCB, 0xCB92, "RES 2, D", 2, 2, Res<2, RegisterName::D>);
Instruction const INSTR_CB93(0xCB, 0xCB93, "RES 2, E", 2, 2, Res<2, RegisterName::E>);
Instruction const INSTR_CB94(0xCB, 0xCB94, "RES 2, H", 2, 2, Res<2, RegisterName::H>);
Instruction const INSTR_CB95(0xCB, 0xCB95, "RES 2, L", 2, 2, Res<2, RegisterName::L>);
Instruction const INSTR_CB96(0xCB, 0xCB96, "RES 2, (HL)", 2, 4, Res<2, RegisterName::HL>);
Instruction const INSTR_CB97(0xCB, 0xCB97, "RES 2, A", 2, 2, Res<2, RegisterName::A>);

Instruction const INSTR_CB98(0xCB, 0xCB98, "RES 3, B", 2, 2, Res<3, RegisterName::B>);
Instruction const INSTR_CB99(0xCB, 0xCB99, "RES 3, C", 2, 2, Res<3, RegisterName::C>);
Instruction const INSTR_CB9A(0xCB, 0xCB9A, "RES 3, D", 2, 2, Res<3, RegisterName::D>);
Instruction const INSTR_CB9B(0xCB, 0xCB9B, "RES 3, E", 2, 2, Res<3, RegisterName::E>);
Instruction const INSTR_CB9C(0xCB, 0xCB9C, "RES 3, H", 2, 2, Res<3, RegisterName::H>);
Instruction const INSTR_CB9D(0xCB, 0xCB9D, "RES 3, L", 2, 2, Res<3, RegisterName::L>);
Instruction const INSTR_CB9E(0xCB, 0xCB9E, "RES 3, (HL)", 2, 4, Res<3, RegisterName::HL>);
Instruction const INSTR_CB9F(0xCB, 0xCB9F, "RES 3, A", 2, 2, Res<3, RegisterName::A>);

Instruction const INSTR_CBA0(0xCB, 0xCBA0, "RES 4, B", 2, 2, Res<4, RegisterName::B>);
Instruction const INSTR_CBA1(0xCB, 0xCBA1, "RES 4, C", 2, 2, Res<4, RegisterName::C>);
Instruction const INSTR_CBA2(0xCB, 0xCBA2, "RES 4, D", 2, 2, Res<4, RegisterName::D>);
Instruction const INSTR_CBA3(0xCB, 0xCBA3, "RES 4, E", 2, 2, Res<4, RegisterName::E>);
Instruction const INSTR_CBA4(0xCB, 0xCBA4, "RES 4, H", 2, 2, Res<4, RegisterName::H>);
Instruction const INSTR_CBA5(0xCB, 0xCBA5, "RES 4, L", 2, 2, Res<4, RegisterName::L>);
Instruction const INSTR_CBA6(0xCB, 0xCBA6, "RES 4, (HL)", 2, 4, Res<4, RegisterName::HL>);
Instruction const INSTR_CBA7(0xCB, 0xCBA7, "RES 4, A", 2, 2, Res<4, RegisterName::A>);

Instruction const INSTR_CBA8(0xCB, 0xCBA8, "RES 5, B", 2, 2, Res<5, RegisterName::B>);
Instruction const INSTR_CBA9(0xCB, 0xCBA9, "RES 5, C", 2, 2, Res<5, RegisterName::C>);
Instruction const INSTR_CBAA(0xCB, 0xCBAA, "RES 5, D", 2, 2, Res<5, RegisterName::D>);
Instruction const INSTR_CBAB(0xCB, 0xCBAB, "RES 5, E", 2, 2, Res<5, RegisterName::E>);
Instruction const INSTR_CBAC(0xCB, 0xCBAC, "RES 5, H", 2, 2, Res<5, RegisterName::H>);
Instruction const INSTR_CBAD(0xCB, 0xCBAD, "RES 5, L", 2, 2, Res<5, RegisterName::L>);
Instruction const INSTR_CBAE(0xCB, 0xCBAE, "RES 5, (HL)", 2, 4, Res<5, RegisterName::HL>);
Instruction const INSTR_CBAF(0xCB, 0xCBAF, "RES 5, A", 2, 2, Res<5, RegisterName::A>);

Instruction const INSTR_CBB0(0xCB, 0xCBB0, "RES 6, B", 2, 2, Res<6, RegisterName::B>);
Instruction const INSTR_CBB1(0xCB, 0xCBB1, "RES 6, C", 2, 2, Res<6, RegisterName::C>);
Instruction const INSTR_CBB2(0xCB, 0xCBB2, "RES 6, D", 2, 2, Res<6, RegisterName::D>);
Instruction const INSTR_CBB3(0xCB, 0xCBB3, "RES 6, E", 2, 2, Res<6, RegisterName::E>);
Instruction const INSTR_CBB4(0xCB, 0xCBB4, "RES 6, H", 2, 2, Res<6, RegisterName::H>);
Instruction const INSTR_CBB5(0xCB, 0xCBB5, "RES 6, L", 2, 2, Res<6, RegisterName::L>);
Instruction const INSTR_CBB6(0xCB, 0xCBB6, "RES 6, (HL)", 2, 4, Res<6, RegisterName::HL>);
Instruction const INSTR_CBB7(0xCB, 0xCBB7, "RES 6, A", 2, 2, Res<6, RegisterName::A>);

Instruction const INSTR_CBB8(0xCB, 0xCBB8, "RES 7, B", 2, 2, Res<7, RegisterName::B>);
Instruction const INSTR_CBB9(0xCB, 0xCBB9, "RES 7, C", 2, 2, Res<7, RegisterName::C>);
Instruction const INSTR_CBBA(0xCB, 0xCBBA, "RES 7, D", 2, 2, Res<7, RegisterName::D>);
Instruction const INSTR_CBBB(0xCB, 0xCBBB, "RES 7, E", 2, 2, Res<7, RegisterName::E>);
Instruction const INSTR_CBBC(0xCB, 0xCBBC, "RES 7, H", 2, 2, Res<7, RegisterName::H>);
Instruction const INSTR_CBBD(0xCB, 0xCBBD, "RES 7, L", 2, 2, Res<7, RegisterName::L>);
Instruction const INSTR_CBBE(0xCB, 0xCBBE, "RES 7, (HL)", 2, 4, Res<7, RegisterName::HL>);
Instruction const INSTR_CBBF(0xCB, 0xCBBF, "RES 7, A", 2, 2, Res<7, RegisterName::A>);

/*


&INSTR_CB80,
&INSTR_CB81,
&INSTR_CB82,
&INSTR_CB83,
&INSTR_CB84,
&INSTR_CB85,
&INSTR_CB86,
&INSTR_CB87,

&INSTR_CB88,
&INSTR_CB89,
&INSTR_CB8A,
&INSTR_CB8B,
&INSTR_CB8C,
&INSTR_CB8D,
&INSTR_CB8E,
&INSTR_CB8F,

&INSTR_CB90,
&INSTR_CB91,
&INSTR_CB92,
&INSTR_CB93,
&INSTR_CB94,
&INSTR_CB95,
&INSTR_CB96,
&INSTR_CB97,

&INSTR_CB98,
&INSTR_CB99,
&INSTR_CB9A,
&INSTR_CB9B,
&INSTR_CB9C,
&INSTR_CB9D,
&INSTR_CB9E,
&INSTR_CB9F,

&INSTR_CBA0,
&INSTR_CBA1,
&INSTR_CBA2,
&INSTR_CBA3,
&INSTR_CBA4,
&INSTR_CBA5,
&INSTR_CBA6,
&INSTR_CBA7,

&INSTR_CBA8,
&INSTR_CBA9,
&INSTR_CBAA,
&INSTR_CBAB,
&INSTR_CBAC,
&INSTR_CBAD,
&INSTR_CBAE,
&INSTR_CBAF,

&INSTR_CBB0,
&INSTR_CBB1,
&INSTR_CBB2,
&INSTR_CBB3,
&INSTR_CBB4,
&INSTR_CBB5,
&INSTR_CBB6,
&INSTR_CBB7,

&INSTR_CBB8,
&INSTR_CBB9,
&INSTR_CBBA,
&INSTR_CBBB,
&INSTR_CBBC,
&INSTR_CBBD,
&INSTR_CBBE,
&INSTR_CBBF,
*/

// clang-format off
std::array<Instruction const *, 256> const INSTRUCTIONS_8BIT{
    &INSTR_00,
    &INSTR_01,
    &INSTR_02,
    &INSTR_03,
    &INSTR_04,
    &INSTR_05,
    &INSTR_06,
    &INSTR_07,
    &INSTR_INVALID,
    &INSTR_09,
    &INSTR_0A,
    &INSTR_0B,
    &INSTR_0C,
    &INSTR_0D,
    &INSTR_0E,
    &INSTR_INVALID, //0F
    &INSTR_INVALID, //10
    &INSTR_11,
    &INSTR_12,
    &INSTR_13,
    &INSTR_14,
    &INSTR_15, //15
    &INSTR_16,
    &INSTR_17,
    &INSTR_18,
    &INSTR_19,
    &INSTR_1A, //1A
    &INSTR_1B,
    &INSTR_1C,
    &INSTR_1D,
    &INSTR_1E,
    &INSTR_INVALID, //1F
    &INSTR_20, //20
    &INSTR_21,
    &INSTR_22,
    &INSTR_23,
    &INSTR_24,
    &INSTR_25, //25
    &INSTR_26,
    &INSTR_27,
    &INSTR_28,
    &INSTR_29,
    &INSTR_2A, //2A
    &INSTR_2B,
    &INSTR_2C,
    &INSTR_2D,
    &INSTR_2E,
    &INSTR_2F, //2F
    &INSTR_30, //30
    &INSTR_31,
    &INSTR_32,
    &INSTR_33,
    &INSTR_34,
    &INSTR_35, //35
    &INSTR_36,
    &INSTR_INVALID,
    &INSTR_38,
    &INSTR_39,
    &INSTR_3A, //3A
    &INSTR_3B,
    &INSTR_3C,
    &INSTR_3D,
    &INSTR_3E,
    &INSTR_INVALID, //3F
    &INSTR_40, //40
    &INSTR_41,
    &INSTR_42,
    &INSTR_43,
    &INSTR_44,
    &INSTR_45, //45
    &INSTR_46,
    &INSTR_47,
    &INSTR_48,
    &INSTR_49,
    &INSTR_4A, //4A
    &INSTR_4B,
    &INSTR_4C,
    &INSTR_4D,
    &INSTR_4E,
    &INSTR_4F, //4F
    &INSTR_50, //50
    &INSTR_51,
    &INSTR_52,
    &INSTR_53,
    &INSTR_54,
    &INSTR_55, //55
    &INSTR_56,
    &INSTR_57,
    &INSTR_58,
    &INSTR_59,
    &INSTR_5A, //5A
    &INSTR_5B,
    &INSTR_5C,
    &INSTR_5D,
    &INSTR_5E,
    &INSTR_5F, //5F
    &INSTR_60, //60
    &INSTR_61,
    &INSTR_62,
    &INSTR_63,
    &INSTR_64,
    &INSTR_65, //65
    &INSTR_66,
    &INSTR_67,
    &INSTR_68,
    &INSTR_69,
    &INSTR_6A, //6A
    &INSTR_6B,
    &INSTR_6C,
    &INSTR_6D,
    &INSTR_6E,
    &INSTR_6F, //6F
    &INSTR_70, //70
    &INSTR_71,
    &INSTR_72,
    &INSTR_73,
    &INSTR_74,
    &INSTR_75, //75
    &INSTR_INVALID,
    &INSTR_77,
    &INSTR_78,
    &INSTR_79,
    &INSTR_7A, //7A
    &INSTR_7B,
    &INSTR_7C,
    &INSTR_7D,
    &INSTR_7E,
    &INSTR_7F, //7F
    &INSTR_80, //80
    &INSTR_81,
    &INSTR_82,
    &INSTR_83,
    &INSTR_84,
    &INSTR_85, //85
    &INSTR_86,
    &INSTR_87,
    &INSTR_88,
    &INSTR_89,
    &INSTR_8A, //8A
    &INSTR_8B,
    &INSTR_8C,
    &INSTR_8D,
    &INSTR_8E,
    &INSTR_8F, //8F
    &INSTR_90, //90
    &INSTR_91,
    &INSTR_92,
    &INSTR_93,
    &INSTR_94,
    &INSTR_95, //95
    &INSTR_INVALID,
    &INSTR_97,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID, //9A
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID, //9F
    &INSTR_A0, //A0
    &INSTR_A1,
    &INSTR_A2,
    &INSTR_A3,
    &INSTR_A4,
    &INSTR_A5, //A5
    &INSTR_INVALID,
    &INSTR_A7,
    &INSTR_A8,
    &INSTR_A9,
    &INSTR_AA, //AA
    &INSTR_AB,
    &INSTR_AC,
    &INSTR_AD,
    &INSTR_AE,
    &INSTR_AF, //AF
    &INSTR_B0, //B0
    &INSTR_B1,
    &INSTR_B2,
    &INSTR_B3,
    &INSTR_B4,
    &INSTR_B5, //B5
    &INSTR_INVALID,
    &INSTR_B7,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID, //BA
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_BE,
    &INSTR_INVALID, //BF
    &INSTR_C0, //C0
    &INSTR_C1,
    &INSTR_C2,
    &INSTR_C3,
    &INSTR_INVALID,
    &INSTR_C5, //C5
    &INSTR_C6,
    &INSTR_C7,
    &INSTR_C8,
    &INSTR_C9,
    &INSTR_CA, //CA
    &INSTR_CB,
    &INSTR_INVALID,
    &INSTR_CD,
    &INSTR_CE,
    &INSTR_CF, //CF
    &INSTR_D0, //D0
    &INSTR_D1,
    &INSTR_D2,
    &INSTR_D3,
    &INSTR_INVALID,
    &INSTR_D5, //D5
    &INSTR_INVALID,
    &INSTR_D7,
    &INSTR_D8,
    &INSTR_D9,
    &INSTR_DA, //DA
    &INSTR_DB,
    &INSTR_INVALID,
    &INSTR_DD,
    &INSTR_INVALID,
    &INSTR_DF, //DF
    &INSTR_E0, //E0
    &INSTR_E1,
    &INSTR_E2,
    &INSTR_E3,
    &INSTR_E4,
    &INSTR_E5, //E5
    &INSTR_E6,
    &INSTR_E7,
    &INSTR_INVALID,
    &INSTR_E9,
    &INSTR_EA, //EA
    &INSTR_EB,
    &INSTR_EC,
    &INSTR_ED,
    &INSTR_INVALID,
    &INSTR_EF, //EF
    &INSTR_F0, //F0
    &INSTR_F1,
    &INSTR_INVALID,
    &INSTR_F3,
    &INSTR_F4,
    &INSTR_F5, //F5
    &INSTR_INVALID,
    &INSTR_F7,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_FA, //FA
    &INSTR_FB,
    &INSTR_FC,
    &INSTR_FD,
    &INSTR_FE,
    &INSTR_FF, //FF
};
std::array<Instruction const *, 256> const INSTRUCTIONS_16BIT{
    &INSTR_INVALID, // 00
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID, // 05
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID, // 0A
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID, // 0F
    &INSTR_CB10,   // 10
    &INSTR_CB11,
    &INSTR_CB12,
    &INSTR_CB13,
    &INSTR_CB14,
    &INSTR_CB15, // 15
    &INSTR_INVALID,
    &INSTR_CB17,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID, // 1A
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID, // 1F
    &INSTR_CB20, // 20
    &INSTR_CB21,
    &INSTR_CB22,
    &INSTR_CB23,
    &INSTR_CB24,
    &INSTR_CB25, // 25
    &INSTR_INVALID,
    &INSTR_CB27,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID, // 2A
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID, // 2F
    &INSTR_CB30, // 30
    &INSTR_CB31,
    &INSTR_CB32,
    &INSTR_CB33,
    &INSTR_CB34,
    &INSTR_CB35, // 35
    &INSTR_CB36,
    &INSTR_CB37,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID, // 3A
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID, // 3F
    &INSTR_CB40, // 40
    &INSTR_CB41,
    &INSTR_CB42,
    &INSTR_CB43,
    &INSTR_CB44,
    &INSTR_CB45, // 45
    &INSTR_INVALID,
    &INSTR_CB47,
    &INSTR_CB48,
    &INSTR_CB49,
    &INSTR_CB4A, // 4A
    &INSTR_CB4B,
    &INSTR_CB4C,
    &INSTR_CB4D,
    &INSTR_INVALID,
    &INSTR_CB4F, // 4F
    &INSTR_CB50, // 50
    &INSTR_CB51,
    &INSTR_CB52,
    &INSTR_CB53,
    &INSTR_CB54,
    &INSTR_CB55, // 55
    &INSTR_INVALID,
    &INSTR_CB57,
    &INSTR_CB58,
    &INSTR_CB59,
    &INSTR_CB5A, // 5A
    &INSTR_CB5B,
    &INSTR_CB5C,
    &INSTR_CB5D,
    &INSTR_INVALID,
    &INSTR_CB5F, // 5F
    &INSTR_CB60, // 60
    &INSTR_CB61,
    &INSTR_CB62,
    &INSTR_CB63,
    &INSTR_CB64,
    &INSTR_CB65, // 65
    &INSTR_INVALID,
    &INSTR_CB67,
    &INSTR_CB68,
    &INSTR_CB69,
    &INSTR_CB6A, // 6A
    &INSTR_CB6B,
    &INSTR_CB6C,
    &INSTR_CB6D,
    &INSTR_INVALID,
    &INSTR_CB6F, // 6F
    &INSTR_CB70, // 70
    &INSTR_CB71,
    &INSTR_CB72,
    &INSTR_CB73,
    &INSTR_CB74,
    &INSTR_CB75, // 75
    &INSTR_INVALID,
    &INSTR_CB77,
    &INSTR_CB78,
    &INSTR_CB79,
    &INSTR_CB7A, // 7A
    &INSTR_CB7B,
    &INSTR_CB7C,
    &INSTR_CB7D,
    &INSTR_INVALID,
    &INSTR_CB7F, // 7F
    &INSTR_CB80,
    &INSTR_CB81,
    &INSTR_CB82,
    &INSTR_CB83,
    &INSTR_CB84,
    &INSTR_CB85,
    &INSTR_CB86,
    &INSTR_CB87,
    &INSTR_CB88,
    &INSTR_CB89,
    &INSTR_CB8A,
    &INSTR_CB8B,
    &INSTR_CB8C,
    &INSTR_CB8D,
    &INSTR_CB8E,
    &INSTR_CB8F,
    &INSTR_CB90,
    &INSTR_CB91,
    &INSTR_CB92,
    &INSTR_CB93,
    &INSTR_CB94,
    &INSTR_CB95,
    &INSTR_CB96,
    &INSTR_CB97,
    &INSTR_CB98,
    &INSTR_CB99,
    &INSTR_CB9A,
    &INSTR_CB9B,
    &INSTR_CB9C,
    &INSTR_CB9D,
    &INSTR_CB9E,
    &INSTR_CB9F,
    &INSTR_CBA0,
    &INSTR_CBA1,
    &INSTR_CBA2,
    &INSTR_CBA3,
    &INSTR_CBA4,
    &INSTR_CBA5,
    &INSTR_CBA6,
    &INSTR_CBA7,
    &INSTR_CBA8,
    &INSTR_CBA9,
    &INSTR_CBAA,
    &INSTR_CBAB,
    &INSTR_CBAC,
    &INSTR_CBAD,
    &INSTR_CBAE,
    &INSTR_CBAF,
    &INSTR_CBB0,
    &INSTR_CBB1,
    &INSTR_CBB2,
    &INSTR_CBB3,
    &INSTR_CBB4,
    &INSTR_CBB5,
    &INSTR_CBB6,
    &INSTR_CBB7,
    &INSTR_CBB8,
    &INSTR_CBB9,
    &INSTR_CBBA,
    &INSTR_CBBB,
    &INSTR_CBBC,
    &INSTR_CBBD,
    &INSTR_CBBE,
    &INSTR_CBBF,
    &INSTR_INVALID, // C0
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID, // C5
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID, // CA
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID, // CF
    &INSTR_INVALID, // D0
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID, // D5
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID, // DA
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID, // DF
    &INSTR_INVALID, // E0
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID, // E5
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID, // EA
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID, // EF
    &INSTR_INVALID, // F0
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID, // F5
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID, // FA
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID,
    &INSTR_INVALID, // FF
};
// clang-format on

Instruction const * Decode16BitInstruction(u8 opcode)
{
    return INSTRUCTIONS_16BIT[opcode];
}

Instruction const * Decode8BitInstruction(u8 opcode)
{
    return INSTRUCTIONS_8BIT[opcode];
}

Instruction const * DecodeInstruction(u16 opcode)
{
    if ((opcode & 0x00FF) == 0x00CB) {
        auto ret = Decode16BitInstruction(opcode >> 8);
        return ret;
        // return Decode16BitInstruction(opcode >> 8);
    } else {
        auto ret = Decode8BitInstruction(opcode & 0xFF);
        return ret;
        // return Decode8BitInstruction(opcode & 0xFF);
    }
}
}
