#include "Navbar.hh"

#include <imgui.h>

#include "UiCommon.hh"

namespace gb4e::ui
{

void DrawNavbar()
{
    if (!showNavbar) {
        return;
    }
    if (ImGui::BeginMainMenuBar()) {
        ImGui::MenuItem("Registers", nullptr, &showRegisterWatch);
        ImGui::MenuItem("Instructions", nullptr, &showInstructionWatch);
        ImGui::MenuItem("Debugger", nullptr, &showDebugger);
        ImGui::MenuItem("Console", nullptr, &showConsole);
        ImGui::MenuItem("Memory", nullptr, &showMemoryWatch);
        ImGui::MenuItem("Metrics", nullptr, &showMetrics);

        ImGui::EndMainMenuBar();
    }
}
};
