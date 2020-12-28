#pragma once

#include <array>
#include <memory>
#include <optional>
#include <set>
#include <string>

#include "Common.hh"
#include "GbCpuState.hh"

namespace gb4e
{
class RomFile;

class GbCpu
{
public:
    /**
     * bootrom must be 256 bytes long and must be valid for as long as the CPU is running
     */
    static std::optional<GbCpu> Create(size_t bootromSize, u8 const * bootrom);

    /**
     * The CPU will keep a reference to romFile, meaning romFile must be a valid pointer for as long as the CPU is
     * running.
     */
    void LoadRom(RomFile const * romFile);

    void Tick(u64 deltaTimeNs);

    std::string DumpInstructions(u16 startAddress, u16 endAddress);

    GbCpuState const * GetState() const { return state.get(); }

    void AddBreakpoint(u16 breakpoint) { breakpoints.emplace(breakpoint); }
    void RemoveBreakpoint(u16 breakpoint) { breakpoints.erase(breakpoint); }
    std::set<u16> const & GetBreakpoints() const { return breakpoints; }

private:
    GbCpu(size_t bootromSize, u8 const * bootrom);

    std::unique_ptr<GbCpuState> state;

    RomFile const * loadedRom;

    u64 clockTimeNs = 0;
    u64 lastCycleNs = 0;

    // Represents how many cycles the currently executing command takes to execute
    // When QueueInstructionResult is called, waitCycles will be set to InstructionResult::consumedCycles and the
    // InstructionResult will be queued. Every clock cycle waitCycles is reduced by 1. When it hits zero, the queued
    // InstructionResult will be executed.
    u8 waitCycles = 1;

    std::optional<InstructionResult> queuedInstructionResult;

    // If the value of the PC register ever is contained in breakpoints, GbCpu::Tick will return early
    // This is to avoid skipping past breakpoints when emulating multiple instructions in one tick
    std::set<u16> breakpoints;
};
};
