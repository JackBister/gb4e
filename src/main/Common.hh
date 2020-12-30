#pragma once

#include <cstdint>
#include <memory>

using s8 = signed char;
using u8 = unsigned char;
using s16 = int16_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

struct SizedArray {
    SizedArray(size_t size, std::unique_ptr<u8[]> && arr) : size(size), arr(std::move(arr)) {}
    size_t size;
    std::unique_ptr<u8[]> arr;
};

namespace gb4e
{
size_t constexpr MEMORY_SIZE = 0x10000;
size_t constexpr VRAM_SIZE = 0x2000;

int constexpr SCREEN_WIDTH = 160;
int constexpr SCREEN_HEIGHT = 144;

u16 constexpr LCDC_ADDRESS = 0xFF40;

constexpr u32 BIT(u8 bit)
{
    return 1 << bit;
}

template <u8 MinBit, u8 MaxBitInclusive, typename T>
constexpr T BITS(T val)
{
    u8 mask = 0;
    for (u8 i = MinBit; i <= MaxBitInclusive; ++i) {
        mask |= 1 << i;
    }
    return val & mask;
}

u8 constexpr FLAG_ZERO = BIT(7); // 1 if result of instruction was != 0
u8 constexpr FLAG_N = BIT(6);    // 1 if instruction was a subtraction
u8 constexpr FLAG_HC = BIT(5);   // 1 if instruction caused a half carry
u8 constexpr FLAG_C = BIT(4);    // 1 if instruction caused a carry

}
