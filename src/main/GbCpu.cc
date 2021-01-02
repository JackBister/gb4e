#include "GbCpu.hh"

#include <cassert>
#include <chrono>
#include <sstream>

#include "Instruction.hh"
#include "logging/Logger.hh"
#include "romfile/RomFile.hh"
#include "ui/Metrics.hh"

auto const logger = Logger::Create("GbCpu");

namespace gb4e
{

std::optional<GbCpu> GbCpu::Create(size_t bootromSize, u8 const * bootrom, GbModel gbModel, Renderer * renderer)
{
    logger->Infof("CLOCK_FREQUENCY=%zu, CYCLE_DURATION_NS=%zu", CLOCK_FREQUENCY, CYCLE_DURATION_NS);

    return GbCpu(bootromSize, bootrom, gbModel, renderer);
}

void GbCpu::Reset()
{
    this->gpuState->Reset();
    this->state->Reset();
}

void GbCpu::LoadRom(RomFile const * romFile)
{
    this->loadedRom = romFile;
    this->cartridge->LoadRom(romFile);
    /*
    auto const cartridgeType = romFile->GetCartridgeType();

    if (cartridgeType->IsRomOnly()) {
        assert(romFile->GetSize() == 32 * 1024);
        memcpy_s(&this->state->memory[0], MEMORY_SIZE, romFile->GetData(), romFile->GetSize());
    } else if (cartridgeType->IsMbc3()) {
        assert(romFile->GetSize() > 16 * 1024);
        memcpy_s(&this->state->memory[0], MEMORY_SIZE, romFile->GetData(), 16 * 1024);
    } else {
        logger->Errorf("Unhandled cartridgeType=%s", ToString(cartridgeType->GetType()).c_str());
        assert(false);
    }
    */
    // memcpy_s(&this->state->memory[0], MEMORY_SIZE, bootrom, bootromSize);
}

int GbCpu::Tick(u64 deltaTimeNs)
{
    int numCycles = 0;
    auto beforeCycle = std::chrono::high_resolution_clock::now();
    while (deltaTimeNs > 0) {
        auto afterCycle = std::chrono::high_resolution_clock::now();
        auto cycleTimeNs = (afterCycle - beforeCycle).count();
        gb4e::ui::cycleTimeNs = cycleTimeNs;
        beforeCycle = std::chrono::high_resolution_clock::now();
        if (deltaTimeNs > CYCLE_DURATION_NS) {
            clockTimeNs += CYCLE_DURATION_NS;
            deltaTimeNs -= CYCLE_DURATION_NS;
        } else {
            clockTimeNs += deltaTimeNs;
            deltaTimeNs = 0;
        }
        logger->Tracef("TickCycle deltaTimeNs=%zu, clockTimeNs=%zu", deltaTimeNs, clockTimeNs);
        if (clockTimeNs < lastCycleNs + CYCLE_DURATION_NS) {
            logger->Tracef("TickCycle not enough time elapsed to execute cycle, will early out");
            continue;
        }
        lastCycleNs = clockTimeNs;
        numCycles++;
        waitCycles--;
        auto beforeGpu = std::chrono::high_resolution_clock::now();
        apuState->TickCycle();
        gpuState->TickCycle();
        gb4e::ui::gpuCycleTimeNs = (std::chrono::high_resolution_clock::now() - beforeGpu).count();
        if (waitCycles > 0) {
            logger->Tracef("TickCycle waitCycles=%d, early out", waitCycles);
            continue;
        }
        if (queuedInstructionResult.has_value()) {
            logger->Tracef("TickCycle applying instructionResult."); // TODO: InstructionResult::ToString
            auto beforeApply = std::chrono::high_resolution_clock::now();
            ApplyInstructionResult(state.get(), memoryState.get(), queuedInstructionResult.value());
            gb4e::ui::applyTimeNs = (std::chrono::high_resolution_clock::now() - beforeApply).count();
            queuedInstructionResult = {};
        }
        u16 pc = state->Get16BitRegisterValue(GetRegister(RegisterName::PC));
        u16 opcode = memoryState->Read16(pc);
        auto instruction = DecodeInstruction(opcode);
        logger->Tracef(
            "TickCycle pc=%04x, applying opcode=%04x, instruction=%s", pc, opcode, instruction->GetLabel().c_str());
        auto beforeApplier = std::chrono::high_resolution_clock::now();
        queuedInstructionResult = instruction->GetApplier()(state.get(), memoryState.get());
        gb4e::ui::instructionTimeNs = (std::chrono::high_resolution_clock::now() - beforeApplier).count();
        waitCycles = queuedInstructionResult.value().GetConsumedCycles();
        logger->Tracef("TickCycle queued, waitCycles=%u", waitCycles);
        if (breakpoints.find(pc) != breakpoints.end() && numCycles > 1) {
            return numCycles;
        }
    }
    return numCycles;
}

std::string GbCpu::DumpInstructions(u16 startAddress, u16 endAddress)
{
    std::stringstream ss;
    for (u16 i = startAddress; i < endAddress;) {
        u16 opcode;
        if (i == 0xFFFF) {
            opcode = (memoryState->Read(i) << 8) | 0x00;
        } else {
            opcode = memoryState->Read16(i);
        }
        auto instruction = DecodeInstruction(opcode);

        logger->Infof("%x: %x %s", i, opcode, instruction->GetLabel().c_str());
        ss << std::hex << i << ": " << instruction->GetLabel() << '\n';

        i += instruction->GetInstructionSize();
    }
    return ss.str();
}

GbCpu::GbCpu(size_t bootromSize, u8 const * bootrom, GbModel gbModel, Renderer * renderer)
    : apuState(new GbApuState()), gpuState(new GbGpuState(gbModel, renderer)),
      state(new GbCpuState(bootromSize, bootrom)), cartridge(new Cartridge())
{
    this->memoryState = std::make_unique<GbMemoryState>(state.get(), gpuState.get(), apuState.get(), cartridge.get());
    // memcpy_s(&this->state->memory[0], MEMORY_SIZE, bootrom, bootromSize);
}

};