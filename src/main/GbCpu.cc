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

std::optional<GbCpu> GbCpu::Create(size_t bootromSize, u8 const * bootrom, GbModel gbModel, Renderer * renderer,
                                   InputSystem const & inputSystem)
{
    logger->Infof("CLOCK_FREQUENCY=%zu, CYCLE_DURATION_NS=%zu", CLOCK_FREQUENCY, CYCLE_DURATION_NS);

    return GbCpu(bootromSize, bootrom, gbModel, renderer, inputSystem);
}

void GbCpu::Reset()
{
    this->gpuState->Reset();
    this->state->Reset();
}

void GbCpu::LoadRom(RomFile const * romFile)
{
    this->cartridge->LoadRom(romFile);
}

void GbCpu::StepInstruction()
{
    while (!queuedInstructionResult.has_value()) {
        TickCycle();
    }
    int numCyclesToTick = waitCycles;
    for (int i = 0; i < numCyclesToTick; ++i) {
        TickCycle();
    }
}

int GbCpu::Tick(u64 deltaTimeNs)
{
    int numCycles = 0;
    auto joypadTickResult = joypad->Tick();
    if (joypadTickResult.triggerInterrupt) {
        u8 iflags = state->ReadMemory(0xFF0F).value();
        iflags |= BIT(4);
        state->WriteMemory(0xFF0F, iflags);
    }
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
        TickCycle();
        u16 pc = state->Get16BitRegisterValue(GetRegister(RegisterName::PC));
        if (breakpoints.find(pc) != breakpoints.end()) {
            return numCycles;
        }
    }
    return numCycles;
}

void GbCpu::TickCycle()
{
    auto beforeGpu = std::chrono::high_resolution_clock::now();
    apuState->TickCycle();
    GpuTickResult gpuTickResult = gpuState->TickCycle();
    {
        u8 iflags = state->ReadMemory(0xFF0F).value();
        iflags |= gpuTickResult.interrupts;
        state->WriteMemory(0xFF0F, iflags);
    }
    gb4e::ui::gpuCycleTimeNs = (std::chrono::high_resolution_clock::now() - beforeGpu).count();
    --waitCycles;
    if (oamDmaCycles > 0) {
        --oamDmaCycles;
        if (oamDmaCycles == 0) {
            // TODO: This is probably incorrect if you write to FF46 during OAM DMA
            u16 base = state->GetOamDmaLocation();
            for (u16 i = 0; i < 0xA0; ++i) {
                u8 value = memoryState->Read(base + i);
                memoryState->Write(0xFE00 + i, value);
            }
            state->SetOamDmaLocation(0xFFFF);
        }
    }
    if (interruptRoutineCycle != 0xFF) {
        if (interruptRoutineCycle == 3) {
            Register constexpr spReg(RegisterName::SP);
            u16 pc = state->Get16BitRegisterValue(GetRegister(RegisterName::PC));
            u16 sp = state->Get16BitRegisterValue(spReg);
            memoryState->Write(sp - 1, pc >> 8);
            memoryState->Write(sp - 2, pc & 0x00FF);
            state->Set16BitRegisterValue(spReg, sp - 2);
            interruptRoutineCycle++;
        } else if (interruptRoutineCycle == 4) {
            u8 iflags = state->ReadMemory(0xFF0F).value();
            Register constexpr pcReg(RegisterName::PC);
            u8 interruptMask = iflags & state->GetInterruptEnable();
            u8 interruptId = (FindFirstSet(interruptMask) - 1);
            u16 interruptHandlerAddress = 0x40 + interruptId * 8;
            state->Set16BitRegisterValue(pcReg, interruptHandlerAddress);
            state->SetInterruptMasterEnable(false);
            memoryState->Write(0xFF0F, iflags ^ (1 << interruptId));
            interruptRoutineCycle = 0xFF;
            waitCycles = 0;
        } else {
            interruptRoutineCycle++;
        }
        return;
    }
    if (waitCycles > 0 && !gpuTickResult.interrupts) {
        logger->Tracef("TickCycle waitCycles=%d, early out", waitCycles);
        return;
    }
    u16 oamDmaLocBefore = state->GetOamDmaLocation();
    if (queuedInstructionResult.has_value()) {
        logger->Tracef("TickCycle applying instructionResult."); // TODO: InstructionResult::ToString
        auto beforeApply = std::chrono::high_resolution_clock::now();
        ApplyInstructionResult(state.get(), memoryState.get(), queuedInstructionResult.value());
        gb4e::ui::applyTimeNs = (std::chrono::high_resolution_clock::now() - beforeApply).count();
        queuedInstructionResult = {};
    }
    u16 oamDmaLocAfter = state->GetOamDmaLocation();
    if (oamDmaLocAfter != oamDmaLocBefore) {
        oamDmaCycles = 160;
    }
    u8 iflags = state->ReadMemory(0xFF0F).value();
    u8 interruptMask = iflags & state->GetInterruptEnable();
    if (interruptMask && state->GetInterruptMasterEnable()) {
        interruptRoutineCycle = 0;
        return;
    }
    u16 pc = state->Get16BitRegisterValue(GetRegister(RegisterName::PC));

    u16 opcode = memoryState->Read16(pc);
    auto instruction = DecodeInstruction(opcode);
    if (opcode != 0 && instruction->GetInstructionWord() == 0) {
        logger->Infof("opcode decode failed, pc=%04x, opcode=%04x", pc, opcode);
    }
    logger->Tracef(
        "TickCycle pc=%04x, applying opcode=%04x, instruction=%s", pc, opcode, instruction->GetLabel().c_str());
    auto beforeApplier = std::chrono::high_resolution_clock::now();
    queuedInstructionResult = instruction->GetApplier()(state.get(), memoryState.get());
    gb4e::ui::instructionTimeNs = (std::chrono::high_resolution_clock::now() - beforeApplier).count();
    waitCycles = queuedInstructionResult.value().GetConsumedCycles();
    logger->Tracef("TickCycle queued, waitCycles=%u", waitCycles);
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

GbCpu::GbCpu(std::unique_ptr<ApuState> && apuState, std::unique_ptr<GbCpuState> && state,
             std::unique_ptr<GbGpuState> && gpuState, std::unique_ptr<Cartridge> && cartridge,
             std::unique_ptr<GbJoypad> && joypad)
    : apuState(std::move(apuState)), state(std::move(state)), gpuState(std::move(gpuState)),
      cartridge(std::move(cartridge)), memoryState(std::move(memoryState)), joypad(std::move(joypad))
{
    this->memoryState =
        std::make_unique<GbMemoryState>(state.get(), gpuState.get(), apuState.get(), cartridge.get(), joypad.get());
}

GbCpu::GbCpu(size_t bootromSize, u8 const * bootrom, GbModel gbModel, Renderer * renderer,
             InputSystem const & inputSystem)
    : apuState(new GbApuState()), gpuState(new GbGpuState(gbModel, renderer)),
      state(new GbCpuState(bootromSize, bootrom)), cartridge(new Cartridge()), joypad(new GbJoypad(inputSystem))
{
    this->memoryState =
        std::make_unique<GbMemoryState>(state.get(), gpuState.get(), apuState.get(), cartridge.get(), joypad.get());
}

};