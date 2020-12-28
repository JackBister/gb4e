
#include <chrono>
#include <cstdio>
#include <thread>

#include <SDL2/SDL.h>
#include <gl/glew.h>
#include <imgui.h>

#include "GbCpu.hh"
#include "GbRenderer.hh"
#include "InputSystem.hh"
#include "Instruction.hh"
#include "SlurpFile.hh"
#include "logging/Logger.hh"
#include "romfile/RomFileLoader.hh"

#include "ui/Console.hh"
#include "ui/Debugger.hh"
#include "ui/InstructionWatch.hh"
#include "ui/MemoryWatch.hh"
#include "ui/Navbar.hh"
#include "ui/RegisterWatch.hh"
#include "ui/imgui_backend/imgui_impl_opengl3.hh"

using namespace std::chrono_literals;

static const auto logger = Logger::Create("main");

#undef main
int main(int argc, char ** argv)
{
    if (argc < 2) {
        logger->Infof("Usage: %s <romfile>", argv[0]);
        return 0;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        logger->Errorf("Failed to init SDL");
        return 1;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
#ifdef _DEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
    auto sdlWindow =
        SDL_CreateWindow("gb4e", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
    auto glCtx = SDL_GL_CreateContext(sdlWindow);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        logger->Errorf("Failed to init GLEW");
        return 1;
    }

    auto imguiCtx = ImGui::CreateContext();
    if (!ImGui_ImplOpenGL3_Init("#version 150")) {
        logger->Errorf("Failed to init Imgui backend");
        return 1;
    }

    gb4e::GbRenderer gbRenderer;
    gb4e::InputSystem inputSystem;

    gb4e::ui::InitInstructionWatch();

    std::optional<SizedArray> bootrom = gb4e::SlurpFile("bootrom.bin");
    if (!bootrom.has_value()) {
        logger->Errorf("Failed to open bootrom.bin");
        return 1;
    }

    std::optional<gb4e::RomFile> romFileOpt = gb4e::LoadRomFile(argv[1]);
    logger->Infof("After load");
    if (!romFileOpt.has_value()) {
        logger->Errorf("Failed to open ROM file with filename=%s", argv[1]);
        return 1;
    }
    gb4e::RomFile romFile = std::move(romFileOpt.value());

    logger->Infof("%s", romFileOpt.value().ToString().c_str());

    std::optional<gb4e::GbCpu> gbCpuOpt = gb4e::GbCpu::Create(bootrom.value().size, bootrom.value().arr.get());
    if (!gbCpuOpt.has_value()) {
        logger->Errorf("Failed to create GbCpu");
        return 1;
    }

    gb4e::GbCpu gbCpu = std::move(gbCpuOpt.value());

    gbCpu.DumpInstructions(0x00, 0xA8);
    gbCpu.DumpInstructions(0xe0, 0xFF);

    auto lastTick = std::chrono::high_resolution_clock::now();
    while (1) {
        auto & imguiIo = ImGui::GetIO();
        int windowWidth, windowHeight;
        SDL_GetWindowSize(sdlWindow, &windowWidth, &windowHeight);
        imguiIo.DisplaySize.x = windowWidth;
        imguiIo.DisplaySize.y = windowHeight;
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        inputSystem.Tick();

        gb4e::ui::DrawNavbar();
        gb4e::ui::DrawRegisterWatch(gbCpu.GetState());
        gb4e::ui::DrawInstructionWatch(gbCpu.GetState());
        gb4e::ui::DrawDebugger(&gbCpu);
        gb4e::ui::DrawConsole();
        gb4e::ui::DrawMemoryWatch(gbCpu.GetState());

        ImGui::Render();
        SDL_GetWindowSize(sdlWindow, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);
        glClearColor(0, 0, 0xFF, 0xFF);
        glClear(GL_COLOR_BUFFER_BIT);
        gbRenderer.RenderFramebuffer(gbCpu.GetState());
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(sdlWindow);

        std::this_thread::sleep_until(lastTick + 16ms);
        lastTick = std::chrono::high_resolution_clock::now();
    }

    ImGui_ImplOpenGL3_Shutdown();
}
