#pragma once

namespace love
{
    char * b64_encode(const char * src, size_t srcLength, size_t lineLength, size_t & dstLength);

    char * b64_decode(const char * src, size_t srcLength, size_t & dstLength);
}
