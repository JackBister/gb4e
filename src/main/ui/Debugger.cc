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

bool breakOnDecodeError = false;
char breakpointBuf[BREAKPOINT_BUF_SIZE];

char memoryBreakpointBuf[BREAKPOINT_BUF_SIZE];

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

        if (ImGui::Checkbox("Break on decode error", &breakOnDecodeError)) {
            cpu->SetBreakOnDecodeError(breakOnDecodeError);
        }

        if (ImGui::Button("Step fwd")) {
            cpu->StepInstruction();
        }

        ImGui::InputText("Breakpoint", breakpointBuf, BREAKPOINT_BUF_SIZE);
        ImGui::SameLine();
        if (ImGui::Button("Add Breakpoint")) {
            std::string breakpointString(breakpointBuf);
            if (!breakpointString.empty()) {
                u16 breakpoint = std::stoi(breakpointString, nullptr, 16);
                cpu->AddBreakpoint(breakpoint);
                breakpointBuf[0] = '\0';
            }
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

        ImGui::InputText("Memory Breakpoint", memoryBreakpointBuf, BREAKPOINT_BUF_SIZE);
        ImGui::SameLine();
        if (ImGui::Button("Add Memory Breakpoint")) {
            std::string breakpointString(memoryBreakpointBuf);
            if (!breakpointString.empty()) {
                u16 memoryBreakpoint = std::stoi(breakpointString, nullptr, 16);
                cpu->AddMemoryWriteBreakpoint(memoryBreakpoint);
                memoryBreakpointBuf[0] = '\0';
            }
        }

        std::set<u16> removedMemoryBreakpoints;
        for (auto const breakpoint : cpu->GetMemoryWriteBreakpoints()) {
            ImGui::Text("%04x", breakpoint);
            ImGui::SameLine();
            if (ImGui::SmallButton("Remove")) {
                removedMemoryBreakpoints.emplace(breakpoint);
            }
        }

        for (auto const breakpoint : removedMemoryBreakpoints) {
            cpu->RemoveMemoryWriteBreakpoint(breakpoint);
        }
    }
    ImGui::End();
}
};