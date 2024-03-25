#pragma once

#define LOVE_INT8_MAX   0x7F
#define LOVE_UINT8_MAX  0xFF
#define LOVE_INT16_MAX  0x7FFF
#define LOVE_UINT16_MAX 0xFFFF
#define LOVE_INT32_MAX  0x7FFFFFFF
#define LOVE_UINT32_MAX 0xFFFFFFFF
#define LOVE_INT64_MAX  0x7FFFFFFFFFFFFFFF
#define LOVE_UINT64_MAX 0xFFFFFFFFFFFFFFFF

static inline uint16_t swap_uint16(uint16_t x)
{
    return (x >> 8) | (x << 8);
}

static inline uint32_t swap_uint32(uint32_t x)
{
    return ((x & 0x000000FF) << 24) | ((x & 0x0000FF00) << 8) | ((x & 0x00FF0000) >> 8) |
           ((x & 0xFF000000) >> 24);
}

static inline uint64_t swap_uint64(uint64_t x)
{
    return ((x << 56) & 0xFF00000000000000ULL) | ((x << 40) & 0x00FF000000000000ULL) |
           ((x << 24) & 0x0000FF0000000000ULL) | ((x << 8) & 0x000000FF00000000ULL) |
           ((x >> 8) & 0x00000000FF000000ULL) | ((x >> 24) & 0x0000000000FF0000ULL) |
           ((x >> 40) & 0x000000000000FF00ULL) | ((x >> 56) & 0x00000000000000FFULL);
}
