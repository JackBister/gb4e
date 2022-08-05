#pragma once

#include <array>
#include <thread>

#include "greatest.h"

#include "Common.hh"
#include "GbCpu.hh"
#include "InputSystem.hh"
#include "SlurpFile.hh"
#include "debug/InstructionTrace.hh"
#include "romfile/RomFileLoader.hh"

class BlarggMemoryListener : public gb4e::MemoryListener
{
public:
    void Write(u16 location, u8 value) final override
    {
        if (location == 0xFF01) {
            lastValue = value;
        }
        if (location == 0xFF02 && value == 0x81) {
            printf("%c", lastValue);
        }
    }

private:
    u8 lastValue = 0;
};

TEST Blargg_CPU_Instrs()
{
    gb4e::FakeRenderer renderer;
    std::optional<SizedArray> bootrom = gb4e::SlurpFile("bootrom.bin");
    if (!bootrom.has_value()) {
        FAILm("Failed to load bootrom");
    }
    std::optional<gb4e::RomFile> romFileOpt =
        gb4e::LoadRomFile("src/third_party/gameboy-test-roms/blargg/cpu_instrs/cpu_instrs.gb");
    if (!romFileOpt.has_value()) {
        FAILm("Failed to load cpu_instrs.gb");
    }
    gb4e::RomFile romFile = std::move(romFileOpt.value());
    printf("%s\n", romFile.ToString().c_str());

    auto listener = std::make_shared<BlarggMemoryListener>();
    gb4e::InputSystemFake inputSystem;
    std::optional<gb4e::GbCpu> cpuOpt = gb4e::GbCpu::Create(
        bootrom.value().size, bootrom.value().arr.get(), gb4e::GbModel::DMG, &renderer, inputSystem, {listener});
    if (!cpuOpt.has_value()) {
        FAILm("Failed to create CPU");
    }
    auto cpu = std::move(cpuOpt.value());
    cpu.LoadRom(&romFile);

    cpu.Tick(31 * 1000 * 1000 * 1000); // 31 seconds = 31 billion ns

    // TODO: Assertions

    PASS();
}

SUITE(Test_ROMs)
{
#ifdef RUN_TEST_ROMS
    RUN_TEST(Blargg_CPU_Instrs);
#endif
}
