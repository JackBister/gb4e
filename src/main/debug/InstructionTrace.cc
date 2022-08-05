#include "InstructionTrace.hh"

#include <fstream>

#include <concurrentqueue/concurrentqueue.h>

#include "Instruction.hh"
#include "logging/Logger.hh"

static const auto logger = Logger::Create("TracerThread");

namespace gb4e::debug
{
moodycamel::ConcurrentQueue<TraceData> queue;
void PushTrace(TraceData const & data)
{
    queue.enqueue(data);
}

void TracerThread(std::filesystem::path const & outFile, std::atomic_bool & isShuttingDown)
{
    logger->Infof("Starting tracer thread with outFile=%ls", outFile.c_str());
    std::ofstream outStream(outFile.c_str());

    size_t i = 0;
    TraceData data;
    while (true) {
        if (isShuttingDown.load()) {
            break;
        }
        if (queue.try_dequeue(data)) {
            outStream << std::hex << "A:" << (int)data.a << " F:" << FlagsToString(data.f) << " BC:" << data.bc
                      << " DE:" << data.de << " HL:" << data.hl << " SP:" << data.sp << " PC:" << data.pc
                      << "(cy: " << std::dec << data.cy << std::hex << ") |[00]0x" << data.pc << ": ";
            auto instr = gb4e::DecodeInstruction(data.instr);
            outStream << instr->GetLabel() << "\n";
        }
        i++;
    }
}
}
