#include <stddef.h>

namespace love
{
    char* Base64Encode(const char* src, size_t srcLength, size_t lineLength,
                       size_t& dstLength);

    char* Base64Decode(const char* src, size_t srcLength, size_t& dstLength);
} // namespace love
