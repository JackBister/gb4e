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
    ss << '\t' << "\"previousValue\": " << std::hex << (int)previousValue << ',';
    ss << '\t' << "\"value\": " << std::hex << (int)value << ' ';
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
    ss << '\t' << "\"previousValue\": " << std::dec << (int)previousValue << ',';
    ss << '\t' << "\"value\": " << std::dec << (int)value << ' ';
    ss << '}';
    return ss.str();
}

std::string RegisterWrite::ToString() const
{
    std::stringstream ss;
    ss << '{';
    ss << '\t' << "\"register\": " << reg.ToString() << ',';
    ss << '\t' << "\"previousValue\": " << std::dec << (int)(reg.Is16Bit() ? wordPreviousValue : bytePreviousValue)
       << ',';
    ss << '\t' << "\"value\": " << std::dec << (int)(reg.Is16Bit() ? wordValue : byteValue) << ' ';
    ss << '}';
    return ss.str();
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
