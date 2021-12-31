#include "common/pixelformat.h"

using namespace love;

unsigned love::GetPixelFormatSize(PixelFormat format)
{
    switch (format)
    {
        case PIXELFORMAT_RGBA8:
        case PIXELFORMAT_RGBA16:
        default:
            return 4;
    }

    return 0;
}

int love::GetPixelFormatColorComponents(PixelFormat format)
{
    switch (format)
    {
        case PIXELFORMAT_RGBA8:
        case PIXELFORMAT_RGBA16:
            return 4;
        default:
            return 0;
    }
}
