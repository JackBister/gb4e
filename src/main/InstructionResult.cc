#include "InstructionResult.hh"

#include <sstream>

#include "logging/Logger.hh"

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
    ss << '\t' << "\"register\": " << reg->ToString() << ',';
    ss << '\t' << "\"previousValue\": " << std::dec << (int)(reg->Is16Bit() ? wordPreviousValue : bytePreviousValue)
       << ',';
    ss << '\t' << "\"value\": " << std::dec << (int)(reg->Is16Bit() ? wordValue : byteValue) << ' ';
    ss << '}';
    return ss.str();
}

std::string InstructionResult::ToString() const
{
    std::stringstream ss;
    ss << '{';
    ss << '\t' << "\"flagSet\": " << (flagSet.has_value() ? flagSet.value().ToString() : "{}") << ',';
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
};
