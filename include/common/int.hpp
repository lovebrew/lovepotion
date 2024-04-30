#pragma once

#include <stdint.h>

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
    return __builtin_bswap16(x);
}

static inline uint32_t swap_uint32(uint32_t x)
{
    return __builtin_bswap32(x);
}

static inline uint64_t swap_uint64(uint64_t x)
{
    return __builtin_bswap64(x);
}

static inline uint16_t swap16_big(uint16_t x)
{
#if defined(LOVE_BIG_ENDIAN)
    return x;
#else
    return swap_uint16(x);
#endif
}
