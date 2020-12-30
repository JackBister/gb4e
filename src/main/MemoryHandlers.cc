#include "MemoryHandlers.hh"

#include "GbCpuState.hh"
#include "logging/Logger.hh"

static auto const logger = Logger::Create("MemoryWriteHandler");

namespace gb4e
{
MemoryWriteHandler ReadonlyWriteHandler()
{
    return [](GbCpuState const *, u16, u8) { return false; };
}

MemoryWriteHandler StubMemoryWriteHandler(std::string name)
{
    return [name](GbCpuState const *, u16 location, u8 value) {
        logger->Warnf("[STUB] Write %s: location=%04x, value=%02x", name.c_str(), location, value);
        return true;
    };
}
};
