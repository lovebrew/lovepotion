#include "seeprom.hpp"

#include <coreinit/bsp.h>

namespace SEEPROM
{
    // https://github.com/wiiu-env/libmocha/blob/89438aa9f3a9d471ff848422f9dc3db581726127/source/utils.cpp#L476
    int read(uint8_t* out_buffer, uint32_t offset, uint32_t size)
    {
        if (!out_buffer || offset > 0x200 || offset & 0x01)
            return -1;

        const auto sizeInShorts   = size >> 1;
        const auto offsetInShorts = offset >> 1;
        const auto maxReadCount   = 0x100 - offsetInShorts;

        if (maxReadCount < sizeInShorts)
            return 0;

        const auto count = sizeInShorts > maxReadCount ? maxReadCount : sizeInShorts;

        auto* ptr  = reinterpret_cast<uint16_t*>(out_buffer);
        int result = 0;

        for (uint32_t index = 0; index < count; index++)
        {
            if (bspRead("EE", offsetInShorts + index, "access", 2, ptr) != 0)
                return -2;
            result += 2;
            ptr++;
        }

        return result;
    }
} // namespace SEEPROM
