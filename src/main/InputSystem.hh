#pragma once

#include <array>

#include "Common.hh"

namespace gb4e
{

enum JoypadButton {
    DPAD_RIGHT = 0,
    DPAD_LEFT = 1,
    DPAD_UP = 2,
    DPAD_DOWN = 3,

    BTN_A = 4,
    BTN_B = 5,
    BTN_SELECT = 6,
    BTN_START = 7,
};

struct InputSystemTickResult {
    bool shouldExit;
};

class InputSystem
{
public:
    virtual void Init() = 0;
    virtual InputSystemTickResult Tick() = 0;

    virtual u8 GetJoypadState() const = 0;
};

class InputSystemFake : public InputSystem
{
public:
    void Init() override {}
    InputSystemTickResult Tick() override { return {.shouldExit = false}; }

    // Bits 0-3=dpad, bits 4-7=buttons
    u8 GetJoypadState() const override { return joypadState; }
    void SetButtonState(JoypadButton btn, bool state)
    {
        if (!state) {
            joypadState |= BIT(btn);
        } else {
            joypadState &= ~BIT(btn);
        }
    }

private:
    u8 joypadState;
};

class InputSystemImpl : public InputSystem
{
public:
    void Init() override;
    InputSystemTickResult Tick() override;

    u8 GetJoypadState() const override { return joypadState; }

private:
    void SetState(JoypadButton btn, bool state);

    u8 joypadState;
};
};