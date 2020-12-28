#include "MemoryWatch.hh"

#include <set>
#include <string>

#include <imgui.h>

#include "Common.hh"
#include "GbCpuState.hh"
#include "UiCommon.hh"
#include "logging/Logger.hh"

static auto const logger = Logger::Create("MemoryWatch");

int constexpr ADDRESS_BUF_SIZE = 12;

namespace gb4e::ui
{

char addressBuf[ADDRESS_BUF_SIZE];
std::set<u16> watchedAddresses;

void DrawMemoryWatch(GbCpuState const * state)
{
    if (!showMemoryWatch) {
        return;
    }

    if (ImGui::Begin("Memory")) {
        ImGui::InputText("Address", addressBuf, ADDRESS_BUF_SIZE);
        if (ImGui::Button("Add watch")) {
            std::string addressString(addressBuf);
            if (!addressString.empty()) {
                u16 address = std::stoi(addressString, nullptr, 16);
                watchedAddresses.emplace(address);
                addressBuf[0] = '\0';
            }
        }

        std::set<u16> removed;
        for (auto const addr : watchedAddresses) {
            u8 value = state->Get8BitMemoryValue(addr);
            ImGui::Text("%04x: %02x", addr, value);
            ImGui::SameLine();
            if (ImGui::SmallButton("Remove")) {
                removed.emplace(addr);
            }
        }
        for (auto const addr : removed) {
            watchedAddresses.erase(addr);
        }
    }
    ImGui::End();
}
};