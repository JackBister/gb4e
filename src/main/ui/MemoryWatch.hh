#pragma once

namespace gb4e
{
class GbCpuState;
class MemoryState;
};

namespace gb4e::ui
{
void DrawMemoryWatch(GbCpuState const *, MemoryState const *);
};
