#pragma once

#include <array>
#include <memory>
#include <optional>
#include <set>
#include <string>

#include "Cartridge.hh"
#include "Common.hh"
#include "GbCpuState.hh"
#include "GbGpuState.hh"
#include "GbJoypad.hh"
#include "MemoryState.hh"
#include "audio/GbApuState.hh"

namespace gb4e
{
class InputSystem;
class Renderer;
class RomFile;

class GbCpu
{
public:
    /**
     * bootrom must be 256 bytes long and must be valid for as long as the CPU is running
     */
    static std::optional<GbCpu> Create(size_t bootromSize, u8 const * bootrom, GbModel gbModel, Renderer * renderer,
                                       InputSystem const & inputSystem,
                                       std::vector<std::shared_ptr<MemoryListener>> listeners = {});
    GbCpu(std::unique_ptr<ApuState> && apuState, std::unique_ptr<GbCpuState> && state,
          std::unique_ptr<GbGpuState> && gpuState, std::unique_ptr<Cartridge> && cartridge,
          std::unique_ptr<GbJoypad> && joypad);

    void Reset();

    /**
     * The CPU will keep a reference to romFile, meaning romFile must be a valid pointer for as long as the CPU is
     * running.
     */
    void LoadRom(RomFile const * romFile);

    void StepInstruction();
    int Tick(u64 deltaTimeNs);
    void TickCycle();

    std::string DumpInstructions(u16 startAddress, u16 endAddress);

    GbCpuState const * GetState() const { return state.get(); }
    GbGpuState const * GetGpu() const { return gpuState.get(); }
    MemoryState const * GetMemory() const { return memoryState.get(); }

    void AddBreakpoint(u16 breakpoint) { breakpoints.emplace(breakpoint); }
    void RemoveBreakpoint(u16 breakpoint) { breakpoints.erase(breakpoint); }
    std::set<u16> const & GetBreakpoints() const { return breakpoints; }

    void AddMemoryWriteBreakpoint(u16 breakpoint) { memoryWriteBreakpoints.emplace(breakpoint); }
    void RemoveMemoryWriteBreakpoint(u16 breakpoint) { memoryWriteBreakpoints.erase(breakpoint); }
    std::set<u16> const & GetMemoryWriteBreakpoints() const { return memoryWriteBreakpoints; }

    void SetBreakOnDecodeError(bool b) { breakOnDecodeError = b; }
    bool GetBreakOnDecodeError() const { return breakOnDecodeError; }

    void SetHistoricInstructionsBufferSize(size_t size)
    {
        historicInstructions.resize(size);
        if (historicInstructionsPtr >= historicInstructions.size()) {
            historicInstructionsPtr = 0;
        }
    }
    std::vector<HistoricInstructionResult> const & GetHistoricInstructionsBuffer() const
    {
        return historicInstructions;
    }
    size_t GetHistoricInstructionsPtr() const { return historicInstructionsPtr; }

    void SetEnableTracing(bool b) { enableTracing = b; }

private:
    GbCpu(size_t bootromSize, u8 const * bootrom, GbModel gbModel, Renderer * renderer, InputSystem const & inputSystem,
          std::vector<std::shared_ptr<MemoryListener>> listeners = {});

    bool IsAtMemoryBreakpoint() const;

    std::unique_ptr<ApuState> apuState;
    std::unique_ptr<GbCpuState> state;
    std::unique_ptr<GbGpuState> gpuState;
    std::unique_ptr<Cartridge> cartridge;
    std::unique_ptr<MemoryState> memoryState;
    std::unique_ptr<GbJoypad> joypad;

    u64 clockTimeNs = 0;
    u64 lastCycleNs = 0;
    u64 totalCycles = 0;

    // Represents how many cycles the currently executing command takes to execute
    // When QueueInstructionResult is called, waitCycles will be set to InstructionResult::consumedCycles and the
    // InstructionResult will be queued. Every clock cycle waitCycles is reduced by 1. When it hits zero, the queued
    // InstructionResult will be executed.
    u8 waitCycles = 1;

    // If this is != 0xFF, an interrupt is currently being executed. When an interrupt is raised this is set to 0 and
    // incremented by 1 per cycle. An interrupt takes 5 cycles. The first 3 cycles do nothing. On cycle 4 IME is set to
    // false and the current PC is pushed. On cycle 5 the PC is set to the interrupt handler.
    u8 interruptRoutineCycle = 0xFF;
    u16 queuedInterruptAddress = 0;

    // If not 0, OAM DMA is ongoing from address contained in state->GetOamDmaLocation()
    // When OAM DMA is triggered this starts to tick down from 160
    u8 oamDmaCycles = 0;

    std::optional<InstructionResult> queuedInstructionResult;

    // If the value of the PC register ever is contained in breakpoints, GbCpu::Tick will return early
    // This is to avoid skipping past breakpoints when emulating multiple instructions in one tick
    std::set<u16> breakpoints;

    // If a memory location contained in memoryWriteBreakpoints is written to, the CPU will pause before executing the
    // instruction
    std::set<u16> memoryWriteBreakpoints;

    // If true and the CPU encounters an opcode which decodes to INSTR_INVALID, the current PC will be added to
    // breakpoints
    bool breakOnDecodeError = false;

    std::vector<HistoricInstructionResult> historicInstructions;
    size_t historicInstructionsPtr;

    bool enableTracing = false;
};
};
