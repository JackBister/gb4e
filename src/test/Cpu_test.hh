#pragma once

#include "greatest.h"

#include "GbCpu.hh"
#include "GbCpuState.hh"
#include "InputSystem.hh"
#include "Renderer.hh"

TEST ApplyInstructionResult_AppliesPendingIme()
{
    using namespace gb4e;

    MemoryStateFake memory;
    GbCpuState state;
    ASSERT_EQ(false, state.GetInterruptMasterEnable());

    ApplyInstructionResult(&state, &memory, InstructionResult(InterruptSet(false, true, true), 1, 1));
    ASSERT_EQ(false, state.GetInterruptMasterEnable());
    ApplyInstructionResult(&state, &memory, InstructionResult());

    ASSERT_EQ(true, state.GetInterruptMasterEnable());
    PASS();
}

TEST Interrupt_Vblank_ImeOff()
{
    using namespace gb4e;

    InputSystemFake inputSystem;
    GbCpu gbCpu(std::make_unique<ApuStateFake>(),
                std::make_unique<GbCpuState>(),
                std::make_unique<GbGpuState>(GbModel::DMG, new FakeRenderer()),
                std::make_unique<Cartridge>(),
                std::make_unique<GbJoypad>(inputSystem));

    // TODO:
    PASS();
}

SUITE(Cpu_test)
{
    RUN_TEST(ApplyInstructionResult_AppliesPendingIme);
}
