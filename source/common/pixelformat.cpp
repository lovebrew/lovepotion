#include "common/pixelformat.h"

using namespace love;

unsigned love::GetPixelFormatSize(PixelFormat format)
{
    switch (format)
    {
        case PIXELFORMAT_R8:
            return 1;
        case PIXELFORMAT_DXT1:
        case PIXELFORMAT_ETC1:
        case PIXELFORMAT_ETC2_RGBA1:
        case PIXELFORMAT_RGBA16:
        case PIXELFORMAT_ETC2_RGB:
            return 8;
        case PIXELFORMAT_DXT3:
        case PIXELFORMAT_DXT5:
        case PIXELFORMAT_ETC2_RGBA:
            return 16;
        case PIXELFORMAT_RGBA8:
        case PIXELFORMAT_TEX3DS_RGBA8:
            return 4;
        case PIXELFORMAT_ASTC_4x4:
        case PIXELFORMAT_ASTC_5x4:
        case PIXELFORMAT_ASTC_6x5:
        case PIXELFORMAT_ASTC_6x6:
        case PIXELFORMAT_ASTC_8x5:
        case PIXELFORMAT_ASTC_8x6:
        case PIXELFORMAT_ASTC_8x8:
        case PIXELFORMAT_ASTC_10x5:
        case PIXELFORMAT_ASTC_10x6:
        case PIXELFORMAT_ASTC_10x8:
        case PIXELFORMAT_ASTC_10x10:
        case PIXELFORMAT_ASTC_12x10:
        case PIXELFORMAT_ASTC_12x12:
            return 1;
        case PIXELFORMAT_RGB8:
            return 3;
        default:
            return 0;
    }
}

int love::GetPixelFormatColorComponents(PixelFormat format)
{
    switch (format)
    {
        case PIXELFORMAT_R8:
            return 1;
        case PIXELFORMAT_DXT1:
        case PIXELFORMAT_ETC1:
        case PIXELFORMAT_ETC2_RGB:
        case PIXELFORMAT_RGB8:
            return 3;
        case PIXELFORMAT_DXT3:
        case PIXELFORMAT_ETC2_RGBA1:
        case PIXELFORMAT_ETC2_RGBA:
        case PIXELFORMAT_DXT5:
        case PIXELFORMAT_RGBA8:
        case PIXELFORMAT_RGBA16:
        case PIXELFORMAT_ASTC_4x4:
        case PIXELFORMAT_ASTC_5x4:
        case PIXELFORMAT_ASTC_6x5:
        case PIXELFORMAT_ASTC_6x6:
        case PIXELFORMAT_ASTC_8x5:
        case PIXELFORMAT_ASTC_8x6:
        case PIXELFORMAT_ASTC_8x8:
        case PIXELFORMAT_ASTC_10x5:
        case PIXELFORMAT_ASTC_10x6:
        case PIXELFORMAT_ASTC_10x8:
        case PIXELFORMAT_ASTC_10x10:
        case PIXELFORMAT_ASTC_12x10:
        case PIXELFORMAT_ASTC_12x12:
        case PIXELFORMAT_TEX3DS_RGBA8:
            return 4;
        default:
            return 0;
    }
}

bool love::IsPixelFormatCompressed(PixelFormat pixelFormat)
{
    int format = static_cast<int>(pixelFormat);

    return format >= static_cast<int>(PIXELFORMAT_DXT1) &&
           format < static_cast<int>(PIXELFORMAT_MAX_ENUM);
}
