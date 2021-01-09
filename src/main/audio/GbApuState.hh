#pragma once

#include <memory>
#include <optional>

#include "Common.hh"

namespace gb4e
{
class AudioPimpl;

class ApuState
{
public:
    virtual void TickCycle() = 0;

    virtual std::optional<u8> ReadMemory(u16 address) const = 0;

    virtual bool WriteMemory(u16 address, u8 value) = 0;
};

class GbApuState final : public ApuState
{
public:
    GbApuState();
    ~GbApuState();

    void TickCycle() final override;

    std::optional<u8> ReadMemory(u16 address) const final override;

    bool WriteMemory(u16 address, u8 value) final override;

private:
    std::unique_ptr<AudioPimpl> pimpl;
};

class ApuStateFake final : public ApuState
{
    void TickCycle() final override {}

    std::optional<u8> ReadMemory(u16 address) const final override { return {}; }

    bool WriteMemory(u16 address, u8 value) final override {}
};
};