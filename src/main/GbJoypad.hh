#pragma once
#include <optional>

#include "Common.hh"

namespace gb4e
{

class InputSystem;

u8 constexpr SELECT_ACTION_BUTTONS = BIT(5);
u8 constexpr SELECT_DPAD_BUTTONS = BIT(4);

struct JoypadTickResult {
    bool triggerInterrupt;
};

class GbJoypad
{
public:
    GbJoypad(InputSystem const & inputSystem) : inputSystem(inputSystem) {}

    JoypadTickResult Tick();

    std::optional<u8> ReadMemory(u16 location) const;
    bool WriteMemory(u16 location, u8 value);

private:
    InputSystem const & inputSystem;

    // 0xFF00
    u8 joypSelect = 0;
    u8 dpad = 0x0F;
    u8 buttons = 0x0F;
};

}
