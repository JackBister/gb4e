#include "InstructionResult.hh"

#include <chrono>
#include <sstream>

#include "GbCpuState.hh"
#include "MemoryState.hh"
#include "logging/Logger.hh"
#include "ui/Metrics.hh"

static auto const logger = Logger::Create("InstructionResult");

namespace gb4e
{
std::string FlagSet::ToString() const
{
    std::stringstream ss;
    ss << '{';
    ss << '\t' << "\"previousValue\": " << FlagsToString(previousValue) << ',';
    ss << '\t' << "\"value\": " << FlagsToString(value) << ' ';
    ss << '}';
    return ss.str();
}

std::string InterruptSet::ToString() const
{
    std::stringstream ss;
    ss << '{';
    ss << '\t' << "\"previousValue\": " << std::hex << (int)previousValue << ',';
    ss << '\t' << "\"value\": " << std::hex << (int)value << ' ';
    ss << '\t' << "\"withInstructionDelay\": " << std::hex << (int)withInstructionDelay << ' ';
    ss << '}';
    return ss.str();
}

std::string MemoryWrite::ToString() const
{
    std::stringstream ss;
    ss << '{';
    ss << '\t' << "\"location\": " << std::hex << (int)location << ',';
    ss << '\t' << "\"previousValue\": " << std::hex << (int)previousValue << ',';
    ss << '\t' << "\"value\": " << std::hex << (int)value << ' ';
    ss << '}';
    return ss.str();
}

std::string RegisterWrite::ToString() const
{
    std::stringstream ss;
    ss << '{';
    ss << '\t' << "\"register\": " << reg.ToString() << ',';
    ss << '\t' << "\"previousValue\": " << std::hex << (int)(reg.Is16Bit() ? wordPreviousValue : bytePreviousValue)
       << ',';
    ss << '\t' << "\"value\": " << std::hex << (int)(reg.Is16Bit() ? wordValue : byteValue) << ' ';
    ss << '}';
    return ss.str();
}

InstructionResult InstructionResult::Reverse() const
{
    std::optional<FlagSet> newFlagSet;
    if (flagSet.has_value()) {
        newFlagSet = FlagSet(flagSet.value().GetValue(), flagSet.value().GetPreviousValue());
    }

    std::optional<InterruptSet> newInterruptSet;
    if (interruptSet.has_value()) {
        newInterruptSet = InterruptSet(
            interruptSet.value().GetValue(),
            interruptSet.value().GetPreviousValue(),
            // TODO: To handle this properly, we need to peek an extra InstructionResult ahead when reverse stepping?
            interruptSet.value().GetWithInstructionDelay());
    }

    std::vector<MemoryWrite> newMemWrites;
    newMemWrites.reserve(memoryWrites.size());
    for (auto const & old : memoryWrites) {
        newMemWrites.emplace_back(old.GetLocation(), old.GetValue(), old.GetPreviousValue());
    }

    std::vector<RegisterWrite> newRegWrites;
    newRegWrites.reserve(registerWrites.size());
    for (auto const & old : registerWrites) {
        if (old.GetRegister().Is16Bit()) {
            newRegWrites.emplace_back(old.GetRegister(), old.GetWordValue(), old.GetWordPreviousValue());
        } else {
            newRegWrites.emplace_back(old.GetRegister(), old.GetByteValue(), old.GetBytePreviousValue());
        }
    }

    // TODO: ... We need to know that these should be minused instead of plussed when stepping backwards
    // TODO: We  also need to think through how these are handled for instructions that affect the PC
    u16 newConsumedBytes = consumedBytes;
    u16 newConsumedCycles = consumedCycles;

    return InstructionResult(
        newFlagSet, newInterruptSet, newMemWrites, newRegWrites, newConsumedBytes, newConsumedCycles);
}

std::string InstructionResult::ToString() const
{
    std::stringstream ss;
    ss << '{';
    ss << '\t' << "\"flagSet\": " << (flagSet.has_value() ? flagSet.value().ToString() : "{}") << ',';
    ss << '\t' << "\"interruptSet\": " << (interruptSet.has_value() ? interruptSet.value().ToString() : "{}") << ',';
    ss << '\t' << "\"memoryWrites\": [\n";
    for (size_t i = 0; i < memoryWrites.size(); ++i) {
        ss << "\t\t" << memoryWrites[i].ToString();
        if (i != memoryWrites.size() - 1) {
            ss << ",\n";
        }
    }
    ss << "\n],";
    ss << '\t' << "\"registerWrites\": [\n";
    for (size_t i = 0; i < registerWrites.size(); ++i) {
        ss << "\t\t" << registerWrites[i].ToString();
        if (i != registerWrites.size() - 1) {
            ss << ",\n";
        }
    }
    ss << "\n],";
    ss << '\t' << "\"consumedBytes\": " << std::hex << (int)consumedBytes << ',';
    ss << '\t' << "\"consumedCycles\": " << std::hex << (int)consumedCycles << '\n';
    ss << '}';
    return ss.str();
}

void ApplyInstructionResult(GbCpuState * cpu, MemoryState * memoryState, InstructionResult const & result)
{
    auto beforeMem = std::chrono::high_resolution_clock::now();
    for (auto const & memWrite : result.GetMemoryWrites()) {
        memoryState->Write(memWrite.GetLocation(), memWrite.GetValue());
    }
    gb4e::ui::applyMemoryTimeNs = (std::chrono::high_resolution_clock::now() - beforeMem).count();

    auto beforeInterrupts = std::chrono::high_resolution_clock::now();
    if (cpu->HasPendingImeEnable()) {
        cpu->SetInterruptMasterEnable(true);
    }
    if (result.GetInterruptSet().has_value()) {
        auto interruptSet = result.GetInterruptSet().value();
        if (interruptSet.GetWithInstructionDelay()) {
            cpu->EnableInterruptsWithDelay();
        } else {
            cpu->SetInterruptMasterEnable(interruptSet.GetValue());
        }
    }
    gb4e::ui::applyInterruptsTimeNs = (std::chrono::high_resolution_clock::now() - beforeInterrupts).count();

    auto beforeFlags = std::chrono::high_resolution_clock::now();
    if (result.GetFlagSet().has_value()) {
        cpu->SetFlags(result.GetFlagSet().value().GetValue());
    }
    gb4e::ui::applyFlagsTimeNs = (std::chrono::high_resolution_clock::now() - beforeFlags).count();

    auto beforeReg = std::chrono::high_resolution_clock::now();
    for (auto const & regWrite : result.GetRegisterWrites()) {
        if (regWrite.GetRegister().Is8Bit()) {
            cpu->Set8BitRegisterValue(regWrite.GetRegister(), regWrite.GetByteValue());
        } else {
            cpu->Set16BitRegisterValue(regWrite.GetRegister(), regWrite.GetWordValue());
        }
    }
    gb4e::ui::applyRegistersTimeNs = (std::chrono::high_resolution_clock::now() - beforeReg).count();

    auto beforePc = std::chrono::high_resolution_clock::now();
    auto pcReg = GetRegister(RegisterName::PC);
    u16 pc = cpu->Get16BitRegisterValue(GetRegister(RegisterName::PC));
    pc += result.GetConsumedBytes();
    cpu->Set16BitRegisterValue(pcReg, pc);
    gb4e::ui::applyPcTimeNs = (std::chrono::high_resolution_clock::now() - beforeReg).count();
}
};
