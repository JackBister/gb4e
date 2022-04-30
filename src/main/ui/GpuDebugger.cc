#include "GpuDebugger.hh"

#include <string>

#include <imgui.h>

#include "GbGpuState.hh"
#include "UiCommon.hh"

namespace gb4e::ui
{
void DrawGpuDebugger(GbGpuState const * gpu)
{
    if (!showGpuDebugger) {
        return;
    }
    if (ImGui::Begin("GPU Debugger")) {

        auto oam = gpu->DebugGetOam();
        for (size_t i = 0; i < oam.size(); ++i) {
            auto str = std::to_string(i);
            if (ImGui::TreeNode(str.c_str())) {
                ImGui::Text("SpriteX: %x", oam[i].spriteX);
                ImGui::Text("SpriteY: %x", oam[i].spriteY);
                ImGui::Text("TileIdx: %x", oam[i].tileIdx);
                ImGui::Text("Attributes: %x", oam[i].attr);
                ImGui::TreePop();
            }
        }
        ImGui::End();
    }
}
}
