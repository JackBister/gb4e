#include "Debugger.hh"

#include <set>

#include <imgui.h>

#include "GbCpu.hh"
#include "UiCommon.hh"
#include "logging/Logger.hh"

static auto const logger = Logger::Create("ui::Debugger");

int constexpr BREAKPOINT_BUF_SIZE = 12;

namespace gb4e::ui
{

int deltaTimeNs = 250;

char breakpointBuf[BREAKPOINT_BUF_SIZE];

void DrawDebugger(GbCpu * cpu)
{
    if (!showDebugger) {
        return;
    }
    if (ImGui::Begin("Debugger")) {
        auto regPc = GetRegister(RegisterName::PC);
        auto const & breakpoints = cpu->GetBreakpoints();
        if (breakpoints.find(cpu->GetState()->Get16BitRegisterValue(regPc)) != breakpoints.end()) {
            isRunning = false;
        }

        if (isRunning) {
            if (ImGui::Button("Break")) {
                isRunning = false;
            }
        } else {
            if (ImGui::Button("Run")) {
                isRunning = true;
            }
            if (ImGui::Button("Reset")) {
                cpu->Reset();
            }
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