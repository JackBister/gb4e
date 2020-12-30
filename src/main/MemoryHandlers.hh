#pragma once

#include <functional>
#include <string>

#include "Common.hh"

namespace gb4e
{
class GbCpuState;

using MemoryWriteHandler = std::function<bool(GbCpuState *, u16, u8)>;

MemoryWriteHandler ReadonlyWriteHandler();
MemoryWriteHandler StubMemoryWriteHandler(std::string name);
};
