#pragma once

#include <atomic>
#include <filesystem>

#include "Common.hh"

namespace gb4e::debug
{
struct TraceData {
    u8 a;
    u8 f;
    u16 bc;
    u16 de;
    u16 hl;
    u16 sp;
    u16 pc;
    u64 cy;
    // TODO: include ROM bank
    u16 instr;
};

void PushTrace(TraceData const &);
void TracerThread(std::filesystem::path const & outFile, std::atomic_bool & isShuttingDown);
}
