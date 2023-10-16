#pragma once

#include <stddef.h>
#include <stdint.h>

namespace love
{
    static constexpr const char hexChars[] = "0123456789abcdef";

    uint8_t Nibble(char c);

    char* BytesToHex(const uint8_t* source, size_t sourceLength,
                     size_t& destinationLength);

    uint8_t* HexToBytes(const char* source, size_t sourceLength,
                        size_t& destinationLength);
} // namespace love
