#include "InputSystem.hh"

#include <SDL2/SDL.h>
#include <imgui.h>

namespace gb4e
{
void InputSystemImpl::Init()
{
    auto & imguiIo = ImGui::GetIO();

    imguiIo.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
    imguiIo.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
    imguiIo.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
    imguiIo.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
    imguiIo.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
    imguiIo.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
    imguiIo.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;

    imguiIo.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
}

InputSystemTickResult InputSystemImpl::Tick()
{
    auto & imguiIo = ImGui::GetIO();
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            return {.shouldExit = true};
        } else if (e.type == SDL_KEYDOWN) {
            if (!e.key.repeat) {
                imguiIo.KeysDown[e.key.keysym.scancode] = true;
            }
        } else if (e.type == SDL_KEYUP) {
            imguiIo.KeysDown[e.key.keysym.scancode] = false;
        } else if (e.type == SDL_MOUSEWHEEL) {
            if (e.wheel.y > 0)
                imguiIo.MouseWheel += 1;
            if (e.wheel.y < 0)
                imguiIo.MouseWheel -= 1;
            break;
        } else if (e.type == SDL_TEXTINPUT) {
            imguiIo.AddInputCharactersUTF8(e.text.text);
            break;
        }
    }

    int mouseX, mouseY;
    Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
    imguiIo.MousePos = ImVec2(mouseX, mouseY);
    imguiIo.MouseDown[0] = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);
    imguiIo.MouseDown[1] = mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT);

    SetState(DPAD_DOWN, imguiIo.KeysDown[SDL_SCANCODE_DOWN]);
    SetState(DPAD_UP, imguiIo.KeysDown[SDL_SCANCODE_UP]);
    SetState(DPAD_LEFT, imguiIo.KeysDown[SDL_SCANCODE_LEFT]);
    SetState(DPAD_RIGHT, imguiIo.KeysDown[SDL_SCANCODE_RIGHT]);
    SetState(BTN_START, imguiIo.KeysDown[SDL_SCANCODE_RETURN]);
    SetState(BTN_SELECT, imguiIo.KeysDown[SDL_SCANCODE_RSHIFT]);
    SetState(BTN_B, imguiIo.KeysDown[SDL_SCANCODE_Z]);
    SetState(BTN_A, imguiIo.KeysDown[SDL_SCANCODE_X]);

    return {.shouldExit = false};
}

void InputSystemImpl::SetState(JoypadButton btn, bool state)
{
    if (!state) {
        joypadState |= BIT(btn);
    } else {
        joypadState &= ~BIT(btn);
    }
}

};