#include "InstructionHistory.hh"

#include <stdlib.h>

#include <imgui.h>

#include "GbCpu.hh"
#include "UiCommon.hh"

int constexpr HISTORY_BUF_SIZE = 8;
int constexpr INSTRUCTION_STRING_MAX_LENGTH = 1024;
int constexpr NUM_INSTRUCTIONS_SHOWN = 128;

namespace gb4e::ui
{
char historySizeBuf[HISTORY_BUF_SIZE];
char * instructionHistoryStrings[NUM_INSTRUCTIONS_SHOWN];
int currentSelectedInstructionInListBox = 0;

void InitInstructionHistory()
{
    char * instructionsMem = (char *)malloc(NUM_INSTRUCTIONS_SHOWN * INSTRUCTION_STRING_MAX_LENGTH);
    memset(instructionsMem, 0, NUM_INSTRUCTIONS_SHOWN * INSTRUCTION_STRING_MAX_LENGTH);
    for (int i = 0; i < NUM_INSTRUCTIONS_SHOWN; ++i) {
        instructionHistoryStrings[i] = &instructionsMem[i * INSTRUCTION_STRING_MAX_LENGTH];
    }
}

void DrawInstructionHistory(GbCpu * cpu)
{
    if (!showInstructionHistory) {
        return;
    }

    if (ImGui::Begin("Instruction History")) {
        ImGui::InputText("History Size", historySizeBuf, HISTORY_BUF_SIZE);
        ImGui::SameLine();
        if (ImGui::Button("Set")) {
            std::string sizeStr(historySizeBuf);
            if (!sizeStr.empty()) {
                size_t newSize = std::stoi(sizeStr);
                cpu->SetHistoricInstructionsBufferSize(newSize);
            }
        }
        auto const & history = cpu->GetHistoricInstructionsBuffer();
        size_t currentHistory = cpu->GetHistoricInstructionsPtr();

        for (size_t i = 0; i < NUM_INSTRUCTIONS_SHOWN; ++i) {
            size_t currIdx = currentHistory - i;
            if (i > currentHistory) {
                currIdx = history.size() - i + currentHistory;
            }
            if (i >= history.size()) {
                continue;
            }
            if (currIdx >= history.size()) {
                continue;
            }
            auto const & currInstr = history[currIdx];
            if (!currInstr.IsValid()) {
                continue;
            }
            snprintf(instructionHistoryStrings[i],
                     INSTRUCTION_STRING_MAX_LENGTH,
                     "%zu: %s",
                     currInstr.GetTotalCycles(),
                     currInstr.GetResult().ToString().c_str());
        }

        ImGui::PushItemWidth(-1.f);
        ImGui::ListBox("", &currentSelectedInstructionInListBox, instructionHistoryStrings, NUM_INSTRUCTIONS_SHOWN, 20);
        ImGui::PopItemWidth();
    }
    ImGui::End();
}
}
