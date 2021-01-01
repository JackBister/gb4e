#include "Metrics.hh"

#include <imgui.h>

#include "UiCommon.hh"

namespace gb4e::ui
{
u64 cycleTimeNs = 0;
u64 instructionTimeNs = 0;
u64 applyTimeNs = 0;
u64 applyFlagsTimeNs = 0;
u64 applyMemoryTimeNs = 0;
u64 applyRegistersTimeNs = 0;
u64 applyPcTimeNs = 0;
u64 gpuCycleTimeNs = 0;
u64 audioCallbackTimeNs = 0;
int cyclesPerFrame = 0;

void DrawMetrics()
{
    if (!showMetrics) {
        return;
    }
    if (ImGui::Begin("Metrics")) {
        ImGui::Text("cycleTimeNs: %zu", cycleTimeNs);
        ImGui::Text("instructionTimeNs: %zu", instructionTimeNs);
        ImGui::Text("applyTimeNs: %zu", applyTimeNs);
        ImGui::Text("gpuCycleTimeNs: %zu", gpuCycleTimeNs);
        ImGui::Text("cyclesPerFrame %d", cyclesPerFrame);
        ImGui::Text("applyFlagsTimeNs: %zu", applyFlagsTimeNs);
        ImGui::Text("applyMemoryTimeNs: %zu", applyMemoryTimeNs);
        ImGui::Text("applyRegistersTimeNs: %zu", applyRegistersTimeNs);
        ImGui::Text("applyPcTimeNs: %zu", applyPcTimeNs);
        ImGui::Text("audioCallbackTimeNs: %zu", audioCallbackTimeNs);
    }
    ImGui::End();
}
}
