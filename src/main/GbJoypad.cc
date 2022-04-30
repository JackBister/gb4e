#include "GbJoypad.hh"

#include "Common.hh"
#include "InputSystem.hh"

namespace gb4e
{
JoypadTickResult GbJoypad::Tick()
{
    u8 state = inputSystem.GetJoypadState();
    u8 newDpad = state & 0xF;
    u8 newButtons = (state & 0xF0) >> 4;
    bool triggerInterrupt = false;
    if (state & SELECT_DPAD_BUTTONS) {
        if ((dpad & newDpad) != dpad) {
            triggerInterrupt = true;
        }
    }
    if (state & SELECT_ACTION_BUTTONS) {
        if ((buttons & newButtons) != buttons) {
            triggerInterrupt = true;
        }
    }
    dpad = newDpad;
    buttons = newButtons;
    return JoypadTickResult{.triggerInterrupt = triggerInterrupt};
}

std::optional<u8> GbJoypad::ReadMemory(u16 location) const
{
    if (location == 0xFF00) {
        if (joypSelect == 0b00110000) {
            return 0xCF;
        }
        if (!(joypSelect & SELECT_DPAD_BUTTONS)) {
            return 0xC0 | joypSelect | dpad;
        }
        if (!(joypSelect & SELECT_ACTION_BUTTONS)) {
            return 0xC0 | joypSelect | buttons;
        }
        return 0xC0 | joypSelect | dpad | buttons;
    }
    return std::nullopt;
}

bool GbJoypad::WriteMemory(u16 location, u8 value)
{
    if (location == 0xFF00) {
        joypSelect = BITS<4, 5>(value);
        return true;
    }
    return false;
}

}