#include "InputSystem.hh"

#include <SDL2/SDL.h>
#include <imgui.h>

namespace gb4e
{
void InputSystem::Tick()
{
    auto & imguiIo = ImGui::GetIO();
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            exit(0);
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
}
};