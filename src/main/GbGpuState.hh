#pragma once

#include <array>
#include <optional>

#include "Common.hh"

namespace gb4e
{
class Renderer;

int constexpr BGPD_SIZE = 64;
int constexpr OAM_SIZE = 160;

int constexpr OAM_READ_CYCLES = 80;
int constexpr VRAM_READ_CYCLES = 172;
int constexpr HBLANK_CYCLES = 204;
int constexpr VBLANK_CYCLES = 456;

// LCDC (FF40)
u8 constexpr LCDC_DISPLAY_ENABLE = BIT(7);    // 0=Off, 1=On
u8 constexpr LCDC_WIN_HIGH_TILEMAP = BIT(6);  // 0=9800-9BFF, 1=9C00-9FFF
u8 constexpr LCDC_WIN_ENABLE = BIT(5);        // 0=Off, 1=On
u8 constexpr LCDC_8000_ADDRESS_MODE = BIT(4); // 0=9000 w/ signed offset, 0=8000 w/unsigned offset
u8 constexpr LCDC_BG_HIGH_TILEMAP = BIT(3);   // 0=9800-9BFF, 1=9C00-9FFF
u8 constexpr LCDC_SPRITE_SIZE = BIT(2);       // 0=8x8, 1=8x16
u8 constexpr LCDC_SPRITE_ENABLE = BIT(1);     // 0=Off, 1=On
u8 constexpr LCDC_BG_WIN_PRIORITY = BIT(0);   // 0=No priority, 1=priority from OAM/BG attribute

// BGP Index (FF68)
u8 constexpr BGPI_AUTO_INCREMENT = BIT(7); // 0=Off, 1=Increment BGPI after write
u8 constexpr BGPI_INDEX_MAX = BIT(5);

// BG Attributes (VRAM Bank 1)
u8 constexpr BG_OAM_PRIORITY = BIT(7); // 0=OAM priority, 1=BG priority
u8 constexpr BG_VERTICAL_FLIP = BIT(6);
u8 constexpr BG_HORIZONTAL_FLIP = BIT(5);
u8 constexpr BG_TILE_VRAM_BANK = BIT(3); // 0=Bank 0, 1=Bank 1
u8 constexpr BG_PALETTE_NUMBER_MAX = BIT(2);

// OAM Attributes
u8 constexpr OAM_BG_OVER_OBJ = BIT(7);                 // 0=No, 1=BG and Window colors 1-3 over the OBJ
u8 constexpr OAM_Y_FLIP = BIT(6);                      // 0=Normal, 1=Vertically mirrored
u8 constexpr OAM_X_FLIP = BIT(5);                      // 0=Normal, 1=Horizontally mirrored
u8 constexpr OAM_PALETTE_NUMBER = BIT(4);              // **Non CGB Mode Only** (0=OBP0, 1=OBP1)
u8 constexpr OAM_TILE_VRAM_BANK = BIT(3);              // **CGB Mode Only**     (0=Bank 0, 1=Bank 1)
u8 constexpr OAM_PALETTE_NUMBER_CGB = BITS<0, 2>(0xF); // **CGB Mode Only**     (OBP0-7)

enum class GbGpuMode { OAM_READ, VRAM_READ, HBLANK, VBLANK };

struct Background {
    u8 attributes;
    u16 data;
};

struct GpuTickResult {
    // Bit 0: V-blank
    // Bit 1: LCD STAT
    u8 interrupts;
};

struct OamEntry {
    u8 spriteX;
    u8 spriteY;
    u8 tileIdx;
    u8 attr;
};

struct Pixel {
    u8 color;
    u8 colorIndex;
    bool priority;
};

class GbGpuState
{
public:
    GbGpuState(GbModel gbModel, Renderer * renderer);

    void Reset();

    std::optional<u8> ReadMemory(u16 location) const;

    GpuTickResult TickCycle();
    bool WriteMemory(u16 location, u8 value);

    // These are only public for test purposes
    Background LoadTile(u16 tilemapLocation, u16 x, u16 y);
    u16 GetColorIndex(u16 tileRow, u8 xInTile);

    std::array<OamEntry, 40> DebugGetOam() const;

private:
    GpuTickResult CycleOamRead();
    GpuTickResult CycleVramRead();
    GpuTickResult CycleHblank();
    GpuTickResult CycleVblank();

    void DrawScanlinePixel(u8 x);
    Pixel DrawScanlineBackground(u8 x);
    Pixel DrawScanlineSprite(u8 x);

    u16 HorizontalFlip(u16 data);

    GbGpuMode mode = GbGpuMode::HBLANK;
    u32 modeCycles = 0;
    u8 currentScanline = 0;

    // 0xFF40
    u8 lcdc = 0x91;

    // 0xFF42:
    u8 scrollY = 0;
    // 0xFF43:
    u8 scrollX = 0;

    // 0xFF47
    // Unused in GBC mode
    u8 bgp[4];

    // FF4A
    u8 windowY = 0;
    // FF4B
    u8 windowX = 0;

    // FF4F
    u8 vramBank = 0;

    // FF68
    u8 bgpIndex = 0;

    std::array<u8, VRAM_SIZE> bank0 = {0};
    std::array<u8, VRAM_SIZE> bank1 = {0};
    std::array<u8, VRAM_SIZE> & activeBank = bank0;

    std::array<u8, BGPD_SIZE> bgPaletteData = {0};
    std::array<u8, OAM_SIZE> oamData = {0};

    std::array<u32, SCREEN_HEIGHT * SCREEN_WIDTH> framebuffer;

    // When drawing a scanline, the current background tile will be loaded here when it is needed
    Background currentBackground;

    GbModel gbModel;
};
};
