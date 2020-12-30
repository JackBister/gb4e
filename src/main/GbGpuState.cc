#include "GbGpuState.hh"

#include <cassert>

#include "Renderer.hh"
#include "logging/Logger.hh"

static auto const logger = Logger::Create("GbGpuState");

namespace gb4e
{

void GbGpuState::Reset()
{
    lcdc = 0x91;
    scrollY = 0;
    scrollX = 0;
    bgp = 0xfc;
    windowY = 0;
    windowX = 0;
    vramBank = 0;
    activeBank = bank0;
    bgpIndex = 0;
}

void GbGpuState::TickCycle()
{
    switch (mode) {
    case GbGpuMode::OAM_READ:
        CycleOamRead();
        return;
    case GbGpuMode::VRAM_READ:
        CycleVramRead();
        return;
    case GbGpuMode::HBLANK:
        CycleHblank();
        return;
    case GbGpuMode::VBLANK:
        CycleVblank();
        return;
    }
}

std::optional<u8> GbGpuState::ReadMemory(u16 location) const
{
    if (location >= 0x8000 && location <= 0x9FFF) {
        return activeBank[location - 0x8000];
    } else if (location == 0xFF40) {
        return lcdc;
    } else if (location == 0xFF42) {
        return scrollY;
    } else if (location == 0xFF43) {
        return scrollX;
    } else if (location == 0xFF44) {
        return currentScanline;
    } else if (location == 0xFF47) {
        return bgp;
    } else if (location == 0xFF4A) {
        return windowY;
    } else if (location == 0xFF4B) {
        return windowX;
    } else if (location == 0xFF4F) {
        return vramBank;
    } else if (location == 0xFF68) {
        return bgpIndex;
    } else if (location == 0xFF69) {
        // TODO:
        return 0xCD;
    }
    return {};
}

bool GbGpuState::WriteMemory(u16 location, u8 value)
{
    if (location >= 0x8000 && location <= 0x9FFF) {
        activeBank[location - 0x8000] = value;
        return true;
    } else if (location == 0xFF40) {
        lcdc = value;
        return true;
    } else if (location == 0xFF42) {
        scrollY = value;
        return true;
    } else if (location == 0xFF43) {
        scrollX = value;
        return true;
    } else if (location == 0xFF47) {
        bgp = value;
        return true;
    } else if (location == 0xFF4A) {
        windowY = value;
        return true;
    } else if (location == 0xFF4B) {
        windowX = value;
        return true;
    } else if (location == 0xFF4F) {
        if (value & 1) {
            activeBank = bank1;
        } else {
            activeBank = bank0;
        }
        vramBank = value;
        return true;
    } else if (location == 0xFF68) {
        bgpIndex = value;
        return true;
    } else if (location == 0xFF69) {
        bool autoIncrement = bgpIndex & BGPI_AUTO_INCREMENT;
        u8 index = BITS<0, 5>(bgpIndex);
        bgPaletteData[index] = value;
        if (autoIncrement) {
            bgpIndex++;
        }
        return true;
    }
    return false;
}

void GbGpuState::CycleOamRead()
{
    if (modeCycles == OAM_READ_CYCLES) {
        mode = GbGpuMode::VRAM_READ;
        modeCycles = 0;
    } else {
        modeCycles++;
    }
}

void GbGpuState::CycleVramRead()
{
    if (modeCycles < SCREEN_WIDTH) {
        DrawScanlinePixel(modeCycles);
        modeCycles++;
    } else if (modeCycles == VRAM_READ_CYCLES) {
        mode = GbGpuMode::HBLANK;
        modeCycles = 0;
    } else {
        modeCycles++;
    }
}

void GbGpuState::CycleHblank()
{
    if (modeCycles == HBLANK_CYCLES) {
        currentScanline++;
        if (currentScanline == 143) {
            renderer->CopyFramebuffer(framebuffer);
            mode = GbGpuMode::VBLANK;
            modeCycles = 0;
        } else {
            mode = GbGpuMode::OAM_READ;
            modeCycles = 0;
        }
    } else {
        modeCycles++;
    }
}

void GbGpuState::CycleVblank()
{
    if (modeCycles == VBLANK_CYCLES) {
        currentScanline++;
        if (currentScanline == 154) {
            currentScanline = 0;
            mode = GbGpuMode::OAM_READ;
            modeCycles = 0;
        } else {
            modeCycles = 0;
        }
    } else {
        modeCycles++;
    }
}

void GbGpuState::DrawScanlinePixel(u8 x)
{
    assert(x < SCREEN_WIDTH);
    Pixel bg = DrawScanlineBackground(x);

    u16 framebufferIdx = currentScanline * SCREEN_WIDTH + x;
    framebuffer[framebufferIdx] = bg.color | bg.color << 8 | bg.color << 16 | 0xFF << 24;
}

Pixel GbGpuState::DrawScanlineBackground(u8 scanX)
{
    u16 tilemapLocation = lcdc & LCDC_BG_HIGH_TILEMAP ? 0x1C00 : 0x1800;

    u16 y = currentScanline + scrollY;

    u16 x = scanX + scrollX;
    u16 xInTile = BITS<0, 2>(x);

    if (scanX == 0 || xInTile == 0) {
        currentBackground = LoadTile(tilemapLocation, x, y);
    }

    u16 index = GetColorIndex(currentBackground.data, xInTile);

    Pixel ret;
    u8 colors[] = {0xFF, 0xC0, 0x60, 0};
    ret.color = colors[index];
    if (scanX == 128 && currentScanline == 64) {
        ret.color = 0xFF0000FF;
    }
    // bg.palette = index;
    /*
    u8 paletteNumber = BITS<0, BG_PALETTE_NUMBER_MAX>(currentBackground.attributes);
    u8 colorIndex = ((currentBackground.data & (BIT(7) >> tileX)) ? BIT(0) : 0) |
                    (currentBackground.data & (BIT(15) >> tileX) ? BIT(1) : 0);
    u8 finalPaletteIndex = (paletteNumber * 4 + colorIndex) * 2;

    Pixel ret;
    ret.color = bgPaletteData[finalPaletteIndex] | (bgPaletteData[finalPaletteIndex + 1] << 8);
    ret.colorIndex = colorIndex;
    ret.priority = currentBackground.attributes & BIT(15);
    */

    return ret;
}

Background GbGpuState::LoadTile(u16 tilemapLocation, u16 x, u16 y)
{
    Background ret;

    u16 tmaddr = 0x1800 + (((lcdc & LCDC_BG_HIGH_TILEMAP) >> 3) << 10), tdaddr;
    tmaddr += (((y >> 3) << 5) + (x >> 3)) & 0x03ff;
    if ((lcdc & LCDC_8000_ADDRESS_MODE) == 0) {
        tdaddr = 0x1000 + ((s8)bank0[tmaddr] << 4);
    } else {
        tdaddr = 0x0000 + (bank0[tmaddr] << 4);
    }
    tdaddr += (y & 7) << 1;
    ret.data = bank0[tdaddr + 0] << 0;
    ret.data |= bank0[tdaddr + 1] << 8;

    ret.attributes = 0;

    return ret;
    /*
    Background ret;
    u8 tileX = x / 8;
    u8 tileY = y / 8;

    u16 tileOffsetLocation = (tilemapLocation + tileY * 32 + tileX) & 0x03ff;
    // TODO: This is GBC behavior
    ret.attributes = bank1[tileOffsetLocation];

    u8 tileOffset = activeBank[tileOffsetLocation];
    bool isTilebank1 = ret.attributes & BG_TILE_VRAM_BANK;

    u8 tileLocation;
    if (lcdc & LCDC_8000_ADDRESS_MODE) {
        tileLocation = tileOffset * 16;
    } else {
        tileLocation = 0x1000 + ((s8)tileOffset) * 16;
    }

    bool isVerticalFlip = lcdc & BG_VERTICAL_FLIP;

    y = BITS<0, 2>(y);
    if (isVerticalFlip) {
        y ^= 0b00000111;
    }
    tileLocation += y << 1;

    if (isTilebank1) {
        ret.data = bank1[tileLocation] | (bank1[tileLocation + 1] << 8);
    } else {
        ret.data = bank0[tileLocation] | (bank0[tileLocation + 1] << 8);
    }

    bool isHorizontalFlip = lcdc & BG_HORIZONTAL_FLIP;
    if (isHorizontalFlip) {
        ret.data = HorizontalFlip(ret.data);
    }

    return ret;
    */
}

u16 GbGpuState::GetColorIndex(u16 tileRow, u8 xInTile)
{
    u16 index = 0;
    index |= (tileRow & (0x0080 >> xInTile)) ? 1 : 0;
    index |= (tileRow & (0x8000 >> xInTile)) ? 2 : 0;
    return index;
}

u16 GbGpuState::HorizontalFlip(u16 data)
{
    return (data & 0x8080) >> 7 | (data & 0x4040) >> 5 | (data & 0x2020) >> 3 | (data & 0x1010) >> 1 |
           (data & 0x0808) << 1 | (data & 0x0404) << 3 | (data & 0x0202) << 5 | (data & 0x0101) << 7;
}
};