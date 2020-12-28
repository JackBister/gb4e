#include "MemoryHandlers.hh"

#include "GbCpuState.hh"
#include "logging/Logger.hh"

static auto const logger = Logger::Create("MemoryWriteHandler");

namespace gb4e
{

MemoryReadHandler EchoRamReadHandler(s16 offset)
{
    return [offset](GbCpuState const * state, u16 location) {
        u16 echoLocation = location + offset;
        logger->Tracef("Echo RAM read, location=%04x, echoLocation=%04x", location, echoLocation);
        return state->Get8BitMemoryValue(echoLocation);
    };
}

MemoryWriteHandler EchoRamWriteHandler(s16 offset)
{
    return [offset](GbCpuState * state, u16 location, u8 value) {
        u16 echoLocation = location + offset;
        logger->Tracef("Echo RAM write, location=%04x, echoLocation=%04x, value=%02x", location, echoLocation, value);
        state->Set8BitMemoryValue(echoLocation, value);
        return true;
    };
}

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
