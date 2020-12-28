#pragma once

#include "Instruction.hh"

namespace gb4e
{
InstructionApplier Add(RegisterName dstRegName, RegisterName srcRegName);
InstructionApplier AddFromAddrReg(RegisterName dstRegName, RegisterName addrRegName);
InstructionApplier And(RegisterName srcRegName);
InstructionApplier Bit(u8 bit, RegisterName regName);
InstructionApplier CallA16();
InstructionApplier CpD8();
InstructionApplier CpFromMem(RegisterName addrRegName);
InstructionApplier Dec(RegisterName regName);
InstructionApplier Inc(RegisterName regName);
InstructionApplier JrFlagS8(u8 flagmask);
InstructionApplier JrNFlagS8(u8 flagmask);
InstructionApplier JrS8();
InstructionApplier Ld(RegisterName dstRegName, RegisterName srcRegName);
InstructionApplier LdA8();
InstructionApplier LdA16(RegisterName srcRegName);
InstructionApplier LdD8(RegisterName dstRegName);
InstructionApplier LdD16(RegisterName dstRegName);
InstructionApplier LdFromAddrReg(RegisterName dstRegName, RegisterName addrRegName);
InstructionApplier LdFromA8(RegisterName dstRegName);
InstructionApplier LdFromA16(RegisterName dstRegName);
InstructionApplier LdHlIncDecA(s8 modifier);
InstructionApplier LdMemViaReg(RegisterName addrRegName, RegisterName srcRegName);
InstructionApplier Or(RegisterName srcRegName);
InstructionApplier Pop(RegisterName dstRegName);
InstructionApplier Push(RegisterName srcRegName);
InstructionApplier Ret();
InstructionApplier RetNFlag(u8 flagmask);
InstructionApplier Rl(RegisterName regName);
InstructionApplier Rla();
InstructionApplier Sub(RegisterName srcRegName);
InstructionApplier Xor(RegisterName srcRegName);
};