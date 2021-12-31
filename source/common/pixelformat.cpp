#include "common/pixelformat.h"

using namespace love;

unsigned love::GetPixelFormatSize(PixelFormat format)
{
    switch (format)
    {
        case PIXELFORMAT_DXT1:
            return 8;
        case PIXELFORMAT_DXT3:
            return 16;
        case PIXELFORMAT_DXT5:
            return 16;
        case PIXELFORMAT_RGBA8:
            return 4;
        case PIXELFORMAT_RGBA16:
            return 8;
        default:
            return 0;
    }
}

int love::GetPixelFormatColorComponents(PixelFormat format)
{
    switch (format)
    {
        case PIXELFORMAT_DXT1:
            return 3;
        case PIXELFORMAT_DXT3:
            return 4;
        case PIXELFORMAT_DXT5:
            return 4;
        case PIXELFORMAT_RGBA8:
        case PIXELFORMAT_RGBA16:
            return 4;
        default:
            return 0;
    }
}
