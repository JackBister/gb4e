#include "MemoryWatch.hh"

#include <set>
#include <string>

#include <imgui.h>

#include "Common.hh"
#include "GbCpuState.hh"
#include "MemoryState.hh"
#include "UiCommon.hh"
#include "logging/Logger.hh"

static auto const logger = Logger::Create("MemoryWatch");

int constexpr ADDRESS_BUF_SIZE = 12;

namespace gb4e::ui
{

char startAddressBuf[ADDRESS_BUF_SIZE];
char endAddressBuf[ADDRESS_BUF_SIZE];
std::set<u16> watchedAddresses;

void DrawMemoryWatch(GbCpuState const * state, MemoryState const * memory)
{
    if (!showMemoryWatch) {
        return;
    }

    if (ImGui::Begin("Memory")) {
        ImGui::InputText("Start Address", startAddressBuf, ADDRESS_BUF_SIZE);
        ImGui::InputText("End Address", endAddressBuf, ADDRESS_BUF_SIZE);
        if (ImGui::Button("Add watch")) {
            std::string startAddressString(startAddressBuf);
            std::string endAddressString(endAddressBuf);
            if (!startAddressString.empty()) {
                if (!endAddressString.empty()) {
                    u16 startAddress = std::stoi(startAddressString, nullptr, 16);
                    u16 endAddress = std::stoi(endAddressString, nullptr, 16);
                    for (u16 i = startAddress; i < endAddress; i += 2) {
                        watchedAddresses.emplace(i);
                    }
                } else {
                    u16 address = std::stoi(startAddressString, nullptr, 16);
                    watchedAddresses.emplace(address);
                }
                startAddressBuf[0] = '\0';
                endAddressBuf[0] = '\0';
            }
        }

        std::set<u16> removed;
        for (auto const addr : watchedAddresses) {
            u8 lo = memory->Read(addr);
            u8 hi = addr < 0xFFFF ? memory->Read(addr + 1) : 0;
            ImGui::Text("%04x: %02x%02x", addr, lo, hi);
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