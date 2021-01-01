#pragma once

#include <memory>
#include <optional>

#include "Common.hh"

namespace gb4e
{
class AudioPimpl;

class GbApuState
{
public:
    GbApuState();
    ~GbApuState();

    void TickCycle();

    std::optional<u8> ReadMemory(u16 address) const;

    bool WriteMemory(u16 address, u8 value);

private:
    std::unique_ptr<AudioPimpl> pimpl;
};
};