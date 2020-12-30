#pragma once

#include "Common.hh"

namespace gb4e::ui
{
extern u64 cycleTimeNs;
extern u64 instructionTimeNs;
extern u64 applyTimeNs;
extern u64 applyFlagsTimeNs;
extern u64 applyMemoryTimeNs;
extern u64 applyRegistersTimeNs;
extern u64 applyPcTimeNs;
extern u64 gpuCycleTimeNs;
extern int cyclesPerFrame;

void DrawMetrics();
}
