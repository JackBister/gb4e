#pragma once

#include "greatest.h"

#include "GbGpuState.hh"
#include "Renderer.hh"

TEST Gpu_LoadTile()
{
    using namespace gb4e;

    NopRenderer renderer;
    GbGpuState state(&renderer);
    state.WriteMemory(0xFF40, 0x91); // LCDC: Display on, 8000 address mode, BG on
    state.WriteMemory(0x9910, 0x19); // Tile at x: 0x10, y: 0x08 uses tile no 0x19

    // Data for tile 1
    state.WriteMemory(0x8190, 0x00);
    state.WriteMemory(0x8191, 0x3C);
    state.WriteMemory(0x8192, 0x00);
    state.WriteMemory(0x8193, 0x42);
    state.WriteMemory(0x8194, 0x00);
    state.WriteMemory(0x8195, 0xB9);
    state.WriteMemory(0x8196, 0x00);
    state.WriteMemory(0x8197, 0xA5);
    state.WriteMemory(0x8198, 0x00);
    state.WriteMemory(0x8199, 0xB9);
    state.WriteMemory(0x819A, 0x00);
    state.WriteMemory(0x819B, 0xA5);
    state.WriteMemory(0x819C, 0x00);
    state.WriteMemory(0x819D, 0x42);
    state.WriteMemory(0x819E, 0x00);
    state.WriteMemory(0x819F, 0x3C);

    u16 expected[] = {0b0000101010100000,
                      0b0010000000001000,
                      0b1000101010000010,
                      0b1000100000100010,
                      0b1000101010000010,
                      0b1000100000100010,
                      0b0010000000001000,
                      0b0000101010100000};

    u16 received[8];

    for (int y = 0; y < 8; ++y) {
        auto bg = state.LoadTile(0, 128, 64 + y);
        received[y] = 0;

        for (int x = 0; x < 8; ++x) {
            u16 idx = state.GetColorIndex(bg.data, x);
            received[y] |= idx << (14 - x * 2);
        }
    }

    for (int y = 0; y < 8; ++y) {
        ASSERT_EQ(expected[y], received[y]);
    }
    PASS();
}

SUITE(Gpu_test)
{
    RUN_TEST(Gpu_LoadTile);
}
