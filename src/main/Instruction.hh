#pragma once

#include <functional>
#include <string>

#include "Common.hh"
#include "InstructionResult.hh"

namespace gb4e
{
class GbCpuState;
class MemoryState;

using InstructionApplier = std::function<InstructionResult(GbCpuState const *, MemoryState const *)>;

class Instruction
{
public:
    Instruction(u8 instructionByte, u16 instructionWord, std::string label, u8 instructionSize, u8 consumedCycles,
                InstructionApplier applier)
        : instructionByte(instructionByte), instructionWord(instructionWord), label(label),
          instructionSize(instructionSize), consumedCycles(consumedCycles), applier(applier)
    {
    }

    u8 GetInstructionByte() const { return instructionByte; }

    u16 GetInstructionWord() const { return instructionWord; }

    std::string const & GetLabel() const { return label; }

    u8 GetInstructionSize() const { return instructionSize; }

    u8 GetConsumedCycles() const { return consumedCycles; }

    InstructionApplier GetApplier() const { return applier; }

private:
    u8 instructionByte;
    u16 instructionWord;
    std::string label;
    u8 instructionSize;
    u8 consumedCycles;
    InstructionApplier applier;
};

/**
 * For single-byte instructions, the first byte (mask 0xF0) should be filled
 */
Instruction const * DecodeInstruction(u16 opcode);
};
