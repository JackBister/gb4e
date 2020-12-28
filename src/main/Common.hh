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
