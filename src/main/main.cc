
#include <atomic>
#include <chrono>
#include <cstdio>
#include <thread>

#include <SDL2/SDL.h>
#include <gl/glew.h>
#include <imgui.h>

#include "GbCpu.hh"
#include "InputSystem.hh"
#include "Instruction.hh"
#include "Renderer.hh"
#include "SlurpFile.hh"
#include "debug/InstructionTrace.hh"
#include "logging/Logger.hh"
#include "romfile/RomFileLoader.hh"

#include "ui/Console.hh"
#include "ui/Debugger.hh"
#include "ui/GpuDebugger.hh"
#include "ui/InstructionHistory.hh"
#include "ui/InstructionWatch.hh"
#include "ui/MemoryWatch.hh"
#include "ui/Metrics.hh"
#include "ui/Navbar.hh"
#include "ui/RegisterWatch.hh"
#include "ui/UiCommon.hh"
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

#ifdef _WIN32
    // Necessary(?) to get audio to play on Windows
    // At least on my machine it wouldn't play without this. Could let the user set it before running the program but
    // that doesn't seem very user friendly.
    putenv("SDL_AUDIODRIVER=winmm");
#endif

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
    std::atomic_bool isShuttingDown = false;

    gb4e::GbRenderer gbRenderer;
    gb4e::InputSystemImpl inputSystem;
    inputSystem.Init();

    gb4e::ui::InitInstructionHistory();
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

    logger->Infof("%s", romFile.ToString().c_str());

    std::optional<gb4e::GbCpu> gbCpuOpt = gb4e::GbCpu::Create(
        bootrom.value().size, bootrom.value().arr.get(), gb4e::GbModel::DMG, &gbRenderer, inputSystem);
    if (!gbCpuOpt.has_value()) {
        logger->Errorf("Failed to create GbCpu");
        return 1;
    }

    std::optional<std::filesystem::path> traceOutputFilepath;
    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "--tracefile") == 0 && i < (argc - 1)) {
            traceOutputFilepath = argv[i + 1];
            break;
        }
    }

    gb4e::GbCpu gbCpu = std::move(gbCpuOpt.value());

    std::optional<std::thread> tracerThread;
    if (traceOutputFilepath.has_value()) {
        tracerThread = std::thread(gb4e::debug::TracerThread, traceOutputFilepath.value(), std::ref(isShuttingDown));
        gbCpu.SetEnableTracing(true);
    }
    gbCpu.LoadRom(&romFile);

    auto lastTick = std::chrono::high_resolution_clock::now();
    while (1) {
        auto & imguiIo = ImGui::GetIO();
        int windowWidth, windowHeight;
        SDL_GetWindowSize(sdlWindow, &windowWidth, &windowHeight);
        imguiIo.DisplaySize.x = windowWidth;
        imguiIo.DisplaySize.y = windowHeight;
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        {
            auto const inputSystemResult = inputSystem.Tick();
            if (inputSystemResult.shouldExit) {
                break;
            }
        }

        if (gb4e::ui::isRunning) {
            gb4e::ui::cyclesPerFrame = gbCpu.Tick(16666666);
        }

        gb4e::ui::DrawNavbar();
        gb4e::ui::DrawRegisterWatch(gbCpu.GetState());
        gb4e::ui::DrawInstructionHistory(&gbCpu);
        gb4e::ui::DrawInstructionWatch(gbCpu.GetState(), gbCpu.GetMemory());
        gb4e::ui::DrawDebugger(&gbCpu);
        gb4e::ui::DrawGpuDebugger(gbCpu.GetGpu());
        gb4e::ui::DrawConsole();
        gb4e::ui::DrawMemoryWatch(gbCpu.GetState(), gbCpu.GetMemory());
        gb4e::ui::DrawMetrics();

        SDL_GetWindowSize(sdlWindow, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);
        glClearColor(0, 0, 0xFF, 0xFF);
        glClear(GL_COLOR_BUFFER_BIT);
        gbRenderer.Draw();
        if (ImGui::Begin("Screen", nullptr, ImGuiWindowFlags_NoResize)) {
            ImGui::Image((ImTextureID)gbRenderer.GetOutputImage(), ImVec2(800, 600));
            ImGui::End();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(sdlWindow);

        std::this_thread::sleep_until(lastTick + 16ms);
        lastTick = std::chrono::high_resolution_clock::now();
    }

    isShuttingDown.store(true);

    ImGui_ImplOpenGL3_Shutdown();
    if (tracerThread.has_value()) {
        tracerThread.value().join();
    }
}
