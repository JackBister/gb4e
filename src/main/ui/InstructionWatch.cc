#include "InstructionWatch.hh"

#include <imgui.h>

#include "GbCpuState.hh"
#include "Instruction.hh"
#include "MemoryState.hh"
#include "UiCommon.hh"

int constexpr INSTRUCTION_STRING_MAX_LENGTH = 64;
int constexpr NUM_INSTRUCTIONS_SHOWN = 1024;

namespace gb4e::ui
{
using namespace gb4e;

char * instructions[NUM_INSTRUCTIONS_SHOWN];

int currentInstructionIdx = 0;

void InitInstructionWatch()
{
    char * instructionsMem = (char *)malloc(NUM_INSTRUCTIONS_SHOWN * INSTRUCTION_STRING_MAX_LENGTH);
    for (int i = 0; i < NUM_INSTRUCTIONS_SHOWN; ++i) {
        instructions[i] = &instructionsMem[i * INSTRUCTION_STRING_MAX_LENGTH];
    }
}

void DrawInstructionWatch(GbCpuState const * state, MemoryState const * memory)
{
    if (!showInstructionWatch) {
        return;
    }

    if (ImGui::Begin("Instructions")) {
        u16 pc = state->Get16BitRegisterValue(GetRegister(RegisterName::PC));

        u16 startAddr;
        if (pc + NUM_INSTRUCTIONS_SHOWN / 2 > 0xFFFF) {
            startAddr = 0xFFFF - NUM_INSTRUCTIONS_SHOWN;
        } else if (pc - NUM_INSTRUCTIONS_SHOWN / 2 < 0) {
            startAddr = 0;
        } else {
            startAddr = pc - NUM_INSTRUCTIONS_SHOWN / 2;
        }
        int offset = 0;
        for (int i = 0; i < NUM_INSTRUCTIONS_SHOWN; ++i) {
            u16 addr = startAddr + offset;
            if (addr == pc) {
                currentInstructionIdx = i;
            }
            u16 instructionWord = memory->Read16(addr);
            auto instruction = DecodeInstruction(instructionWord);
            snprintf(instructions[i],
                     INSTRUCTION_STRING_MAX_LENGTH,
                     "%04x: %s (%04x)",
                     addr,
                     instruction->GetLabel().c_str(),
                     instruction->GetInstructionWord());
            offset += instruction->GetInstructionSize();
        }

        ImGui::PushItemWidth(-1.f);
        ImGui::ListBox("", &currentInstructionIdx, instructions, NUM_INSTRUCTIONS_SHOWN, 20);
        ImGui::PopItemWidth();
    }
    ImGui::End();
}
};
