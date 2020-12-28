#pragma once

#include <functional>
#include <string>

#include "Common.hh"

namespace gb4e
{
class GbCpuState;

using MemoryReadHandler = std::function<u8(GbCpuState const *, u16)>;
using MemoryWriteHandler = std::function<bool(GbCpuState *, u16, u8)>;

MemoryReadHandler EchoRamReadHandler(s16 offset);

MemoryWriteHandler EchoRamWriteHandler(s16 offset);
MemoryWriteHandler ReadonlyWriteHandler();
MemoryWriteHandler StubMemoryWriteHandler(std::string name);
};
