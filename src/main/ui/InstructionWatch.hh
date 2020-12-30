#pragma once

namespace gb4e
{
class GbCpuState;
class MemoryState;
};

namespace gb4e::ui
{
void InitInstructionWatch();
void DrawInstructionWatch(GbCpuState const *, MemoryState const *);
};
