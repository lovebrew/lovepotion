#pragma once

#if defined (_3DS)
    #include <3ds/types.h>
#elif defined (__SWITCH__)
    #include <switch/types.h>
#endif

namespace love
{
    char * b64_encode(const char * src, size_t srcLength, size_t lineLength, size_t & dstLength);

    char * b64_decode(const char * src, size_t srcLength, size_t & dstLength);
}
