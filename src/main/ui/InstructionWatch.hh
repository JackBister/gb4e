#pragma once

namespace gb4e
{
class GbCpuState;
};

namespace gb4e::ui
{
void InitInstructionWatch();
void DrawInstructionWatch(GbCpuState const *);
};
