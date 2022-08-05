cmake_minimum_required(VERSION 3.16)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0082 NEW)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(gb4e C CXX)

file(GLOB_RECURSE SOURCES src/main/*.cc src/main/*.hh)
list(FILTER SOURCES EXCLUDE REGEX "src/main/main.cc$")

add_executable(gb4e ${SOURCES} src/main/main.cc)

target_precompile_headers(gb4e PRIVATE
    "src/main/logging/Logger.hh"

   # "imgui.h"
   # "SDL2/SDL.h"

    "<algorithm$<ANGLE-R>"
    "<filesystem$<ANGLE-R>"
    "<functional$<ANGLE-R>"
    "<memory$<ANGLE-R>"
    "<optional$<ANGLE-R>"
    "<string$<ANGLE-R>"
    "<unordered_map$<ANGLE-R>"
    "<variant$<ANGLE-R>"
    "<vector$<ANGLE-R>"
)

set_property(TARGET gb4e PROPERTY CXX_STANDARD 23)
set_property(TARGET gb4e PROPERTY ENABLE_EXPORTS 1)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /UMBCS /D_UNICODE /DUNICODE /std:c++latest")
    set_property(TARGET gb4e PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    add_compile_options("/MP")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2b -lstdc++fs")
endif()

include(FindOpenGL)
find_package(GLEW REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_library(SDL2_LIBRARY SDL2)

find_path(GLEW_INCLUDE_DIR gl/glew.h)
find_path(IMGUI_INCLUDE_DIR imgui.h)
find_path(SDL_INCLUDE_DIR SDL2/SDL.h)

target_include_directories(gb4e PUBLIC src/main)
target_include_directories(gb4e SYSTEM PRIVATE src/third_party ${GLEW_INCLUDE_DIR} ${IMGUI_INCLUDE_DIR} ${SDL_INCLUDE_DIR})

target_link_libraries(
        gb4e
        ${GLEW_LIBRARY}
        ${OPENGL_LIBRARIES}
        imgui::imgui
        ${SDL2_LIBRARY}
)

file(GLOB_RECURSE TEST_SOURCES src/test/*.cc src/test/*.hh)

add_executable(gb4e_tests ${TEST_SOURCES} ${SOURCES})
target_include_directories(gb4e_tests PUBLIC src/main)
target_include_directories(gb4e_tests SYSTEM PRIVATE ${GLEW_INCLUDE_DIR} ${IMGUI_INCLUDE_DIR} ${SDL_INCLUDE_DIR})

target_link_libraries(
        gb4e_tests
        ${GLEW_LIBRARY}
        ${OPENGL_LIBRARIES}
        imgui::imgui
        ${SDL2_LIBRARY}
)

option(RUN_TEST_ROMS "Whether the test suite should include tests that require test ROMs to be downloaded. In order for this to work you must have run ./download-test-roms.sh before running the tests." OFF)
if (RUN_TEST_ROMS)
    target_compile_definitions(gb4e_tests PUBLIC -DRUN_TEST_ROMS)
endif()
