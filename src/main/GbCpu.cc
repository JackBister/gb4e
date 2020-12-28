#include "GbCpu.hh"

#include <cassert>
#include <sstream>

#include "Instruction.hh"
#include "logging/Logger.hh"
#include "romfile/RomFile.hh"

auto const logger = Logger::Create("GbCpu");

u64 constexpr CLOCK_FREQUENCY = 4194304;
u64 constexpr CYCLE_DURATION_NS = 1000000000 / CLOCK_FREQUENCY;

namespace gb4e
{

std::optional<GbCpu> GbCpu::Create(size_t bootromSize, u8 const * bootrom)
{
    logger->Infof("CLOCK_FREQUENCY=%zu, CYCLE_DURATION_NS=%zu", CLOCK_FREQUENCY, CYCLE_DURATION_NS);
    assert(bootromSize == 256);
    if (bootromSize != 256) {
        logger->Errorf("Failed to create GbCpu: bootromSize must be 256.");
        return {};
    }

    return GbCpu(bootromSize, bootrom);
}

void GbCpu::LoadRom(RomFile const * romFile)
{
    this->loadedRom = romFile;
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
    // memcpy_s(&this->state->memory[0], MEMORY_SIZE, bootrom, bootromSize);
}

void GbCpu::Tick(u64 deltaTimeNs)
{
    int numCycles = 0;
    while (deltaTimeNs > 0) {
        numCycles++;
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
        waitCycles--;
        if (waitCycles > 0) {
            logger->Tracef("TickCycle waitCycles=%d, early out", waitCycles);
            continue;
        }
        if (queuedInstructionResult.has_value()) {
            logger->Tracef("TickCycle applying instructionResult."); // TODO: InstructionResult::ToString
            state->ApplyInstructionResult(queuedInstructionResult.value());
            queuedInstructionResult = {};
        }
        u16 pc = state->Get16BitRegisterValue(GetRegister(RegisterName::PC));
        u16 opcode = state->Get16BitMemoryValue(pc);
        auto instruction = DecodeInstruction(opcode);
        logger->Tracef(
            "TickCycle pc=%04x, applying opcode=%04x, instruction=%s", pc, opcode, instruction->GetLabel().c_str());
        queuedInstructionResult = instruction->GetApplier()(state.get());
        waitCycles = queuedInstructionResult.value().GetConsumedCycles();
        logger->Tracef("TickCycle queued, waitCycles=%u", waitCycles);
        if (breakpoints.find(pc) != breakpoints.end() && numCycles > 1) {
            return;
        }
    }
}

std::string GbCpu::DumpInstructions(u16 startAddress, u16 endAddress)
{
    std::stringstream ss;
    for (u16 i = startAddress; i < endAddress;) {
        u16 opcode;
        if (i == 0xFFFF) {
            opcode = ((u16)this->state->memory[i] << 8) | 0x00;
        } else {
            opcode = ((u16)this->state->memory[i] << 8) | ((u16)this->state->memory[i + 1]);
        }
        auto instruction = DecodeInstruction(opcode);

        logger->Infof("%x: %x %s", i, opcode, instruction->GetLabel().c_str());
        ss << std::hex << i << ": " << instruction->GetLabel() << '\n';

        i += instruction->GetInstructionSize();
    }
    return ss.str();
}

GbCpu::GbCpu(size_t bootromSize, u8 const * bootrom) : state(new GbCpuState(bootromSize, bootrom))
{
    memcpy_s(&this->state->memory[0], MEMORY_SIZE, bootrom, bootromSize);
}

};