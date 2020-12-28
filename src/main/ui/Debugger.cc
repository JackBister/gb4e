#include "Debugger.hh"

#include <imgui.h>
#include <set>

#include "GbCpu.hh"
#include "UiCommon.hh"

int constexpr BREAKPOINT_BUF_SIZE = 12;

namespace gb4e::ui
{

int deltaTimeNs = 250;

char breakpointBuf[BREAKPOINT_BUF_SIZE];

bool isRunningToBreakpoint = false;

void DrawDebugger(GbCpu * cpu)
{
    if (!showDebugger) {
        return;
    }
    if (ImGui::Begin("Debugger")) {
        if (isRunningToBreakpoint) {
            cpu->Tick(10000000);
            auto regPc = GetRegister(RegisterName::PC);
            auto const & breakpoints = cpu->GetBreakpoints();
            if (breakpoints.find(cpu->GetState()->Get16BitRegisterValue(regPc)) != breakpoints.end()) {
                isRunningToBreakpoint = false;
            }
        }

        // For now this ticks the CPU until a breakpoint is hit
        // This is not the way this should be done since it will block the UI thread until the breakpoint is hit
        // which could lead to a hard lock.
        if (ImGui::Button("TODO: Run") && !cpu->GetBreakpoints().empty()) {
            isRunningToBreakpoint = true;
        }

        ImGui::InputInt("elapsed time NS", &deltaTimeNs);
        ImGui::SameLine();
        if (ImGui::Button("Tick")) {
            cpu->Tick(deltaTimeNs);
        }

        ImGui::InputText("Breakpoint", breakpointBuf, BREAKPOINT_BUF_SIZE);
        ImGui::SameLine();
        if (ImGui::Button("Add")) {
            std::string breakpointString(breakpointBuf);
            u16 breakpoint = std::stoi(breakpointString, nullptr, 16);
            cpu->AddBreakpoint(breakpoint);
            breakpointBuf[0] = '\0';
        }

        std::set<u16> removed;
        for (auto const breakpoint : cpu->GetBreakpoints()) {
            ImGui::Text("%04x", breakpoint);
            ImGui::SameLine();
            if (ImGui::SmallButton("Remove")) {
                removed.emplace(breakpoint);
            }
        }

        for (auto const breakpoint : removed) {
            cpu->RemoveBreakpoint(breakpoint);
        }
    }
    ImGui::End();
}
};