#include "GbGpuState.hh"

#include <cassert>

#include "Renderer.hh"
#include "logging/Logger.hh"

static auto const logger = Logger::Create("GbGpuState");

namespace gb4e
{

static u32 const DMG_COLOR_PALETTE[] = {0xFFFFFFFF, 0xFFC0C0C0, 0xFF606060, 0xFF000000};

GbGpuState::GbGpuState(GbModel gbModel, Renderer * renderer) : gbModel(gbModel)
{
    renderer->SetFramebuffer(&this->framebuffer);
    lcdc = 0x91;
    scrollY = 0;
    scrollX = 0;
    bgp[0] = 0b00;
    bgp[1] = 0b11;
    bgp[2] = 0b11;
    bgp[3] = 0b11;
    windowY = 0;
    windowX = 0;
    vramBank = 0;
    activeBank = bank0;
    bgpIndex = 0;
}

void GbGpuState::Reset()
{
    lcdc = 0x91;
    scrollY = 0;
    scrollX = 0;
    bgp[0] = 0b00;
    bgp[1] = 0b11;
    bgp[2] = 0b11;
    bgp[3] = 0b11;
    windowY = 0;
    windowX = 0;
    vramBank = 0;
    activeBank = bank0;
    bgpIndex = 0;
}

GpuTickResult GbGpuState::TickCycle()
{
    switch (mode) {
    case GbGpuMode::OAM_READ:
        return CycleOamRead();
    case GbGpuMode::VRAM_READ:
        return CycleVramRead();
    case GbGpuMode::HBLANK:
        return CycleHblank();
    case GbGpuMode::VBLANK:
        return CycleVblank();
    }
    assert(false);
    return {0};
}

std::optional<u8> GbGpuState::ReadMemory(u16 location) const
{
    if (location >= 0x8000 && location <= 0x9FFF) {
        return activeBank[location - 0x8000];
    } else if (location >= 0xFE00 && location <= 0xFE9F) {
        return oamData[location - 0xFE00];
    } else if (location == 0xFF40) {
        return lcdc;
    } else if (location == 0xFF42) {
        return scrollY;
    } else if (location == 0xFF43) {
        return scrollX;
    } else if (location == 0xFF44) {
        return currentScanline;
    } else if (location == 0xFF47) {
        u8 ret = 0;
        ret |= (bgp[0] << 0) & 0b11;
        ret |= (bgp[1] << 2) & 0b1100;
        ret |= (bgp[2] << 4) & 0b110000;
        ret |= (bgp[3] << 6) & 0b11000000;
        return ret;
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
    } else if (location >= 0xFE00 && location <= 0xFE9F) {
        oamData[location - 0xFE00] = value;
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
        bgp[0] = BITS<0, 1>(value);
        bgp[1] = BITS<2, 3>(value) >> 2;
        bgp[2] = BITS<4, 5>(value) >> 4;
        bgp[3] = BITS<6, 7>(value) >> 6;
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

GpuTickResult GbGpuState::CycleOamRead()
{
    if (modeCycles == OAM_READ_CYCLES) {
        mode = GbGpuMode::VRAM_READ;
        modeCycles = 0;
    } else {
        modeCycles++;
    }
    return {0};
}

GpuTickResult GbGpuState::CycleVramRead()
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
    return {0};
}

GpuTickResult GbGpuState::CycleHblank()
{
    if (modeCycles == HBLANK_CYCLES) {
        currentScanline++;
        if (currentScanline == 143) {
            mode = GbGpuMode::VBLANK;
            modeCycles = 0;
            return {0b01};
        } else {
            mode = GbGpuMode::OAM_READ;
            modeCycles = 0;
        }
    } else {
        modeCycles++;
    }
    return {0};
}

GpuTickResult GbGpuState::CycleVblank()
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
    return {0};
}

void GbGpuState::DrawScanlinePixel(u8 x)
{
    assert(x < SCREEN_WIDTH);

    bool bgWindowEnabled = lcdc & LCDC_BG_WIN_PRIORITY;
    bool spriteEnabled = lcdc & LCDC_SPRITE_ENABLE;

    Pixel bg = DrawScanlineBackground(x);
    Pixel sprite = DrawScanlineSprite(x);

    u16 framebufferIdx = currentScanline * SCREEN_WIDTH + x;
    framebuffer[framebufferIdx] = DMG_COLOR_PALETTE[bg.color];

    if (spriteEnabled && sprite.color != 0) {
        framebuffer[framebufferIdx] = DMG_COLOR_PALETTE[sprite.color];
    }
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
    ret.color = bgp[index];
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

Pixel GbGpuState::DrawScanlineSprite(u8 scanX)
{
    u8 spriteSize = lcdc & LCDC_SPRITE_SIZE;
    u8 spriteHeight = spriteSize ? 16 : 8;

    // TODO: Real hardware is limited to 10 sprites per scanline
    for (int i = 0; i < 160; i += 4) {
        u8 spriteY = oamData[i] - 16;
        u8 spriteX = oamData[i + 1] - 8;
        u8 tileIdx = oamData[i + 2];
        u8 attributes = oamData[i + 3];

        if (currentScanline < spriteY || currentScanline >= (spriteY + spriteHeight)) {
            continue;
        }

        if (scanX < spriteX || scanX >= (spriteX + 8)) {
            continue;
        }

        u8 yInTile = currentScanline - spriteY;
        // TODO: Vertical flip
        yInTile *= 2;
        u16 tdaddr = tileIdx * 16 + yInTile;
        u16 data = bank0[tdaddr];
        data |= bank0[tdaddr + 1] << 8;

        if (attributes & OAM_X_FLIP) {
            data = HorizontalFlip(data);
        }

        u8 xInTile = scanX - spriteX;
        if (xInTile > 7) {
            continue;
        }
        u16 index = GetColorIndex(data, xInTile);

        Pixel ret;
        ret.color = bgp[index];
        return ret;
    }

    Pixel ret;
    ret.color = 0;
    return ret;
}

Background GbGpuState::LoadTile(u16 tilemapLocation, u16 x, u16 y)
{
    Background ret;

    u16 tmaddr = tilemapLocation;
    tmaddr += (((y >> 3) << 5) + (x >> 3)) & 0x03ff;
    u16 tdaddr;
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

std::array<OamEntry, 40> GbGpuState::DebugGetOam() const
{
    std::array<OamEntry, 40> ret;
    for (int i = 0; i < 40; ++i) {
        ret[i].spriteY = oamData[i * 4 + 0];
        ret[i].spriteX = oamData[i * 4 + 1];
        ret[i].tileIdx = oamData[i * 4 + 2];
        ret[i].attr = oamData[i * 4 + 3];
    }
    return ret;
}
};
