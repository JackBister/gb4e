#pragma once

namespace gb4e
{
class GbCpu;
};

namespace gb4e::ui
{
void InitInstructionHistory();
void DrawInstructionHistory(GbCpu *);
};
