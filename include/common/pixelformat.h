#pragma once

namespace love
{
    enum PixelFormat
    {
        PIXELFORMAT_UNKNOWN,

        PIXELFORMAT_NORMAL,

        PIXELFORMAT_TEX3DS_RGBA8,

        // "regular" formats
        PIXELFORMAT_RGB8,

        PIXELFORMAT_RGBA8,
        PIXELFORMAT_RGBA16,

        PIXELFORMAT_R8,

        PIXELFORMAT_RGBA4,
        PIXELFORMAT_RGB565,

        PIXELFORMAT_LA8,

        // depth/stencil
        PIXELFORMAT_STENCIL8,
        PIXELFORMAT_DEPTH16,
        PIXELFORMAT_DEPTH24,
        PIXELFORMAT_DEPTH32F,
        PIXELFORMAT_DEPTH24_STENCIL8,
        PIXELFORMAT_DEPTH2432F_STENCIL8,

        // compressed formats
        PIXELFORMAT_DXT1,
        PIXELFORMAT_DXT3,
        PIXELFORMAT_DXT5,
        PIXELFORMAT_BC4,
        PIXELFORMAT_BC4s,
        PIXELFORMAT_BC5,
        PIXELFORMAT_BC5s,
        PIXELFORMAT_BC6H,
        PIXELFORMAT_BC6Hs,
        PIXELFORMAT_BC7,
        PIXELFORMAT_PVR1_RGB2,
        PIXELFORMAT_PVR1_RGB4,
        PIXELFORMAT_PVR1_RGBA2,
        PIXELFORMAT_PVR1_RGBA4,
        PIXELFORMAT_ETC1,
        PIXELFORMAT_ETC2_RGB,
        PIXELFORMAT_ETC2_RGBA,
        PIXELFORMAT_ETC2_RGBA1,
        PIXELFORMAT_EAC_R,
        PIXELFORMAT_EAC_Rs,
        PIXELFORMAT_EAC_RG,
        PIXELFORMAT_EAC_RGs,
        PIXELFORMAT_ASTC_4x4,
        PIXELFORMAT_ASTC_5x4,
        PIXELFORMAT_ASTC_5x5,
        PIXELFORMAT_ASTC_6x5,
        PIXELFORMAT_ASTC_6x6,
        PIXELFORMAT_ASTC_8x5,
        PIXELFORMAT_ASTC_8x6,
        PIXELFORMAT_ASTC_8x8,
        PIXELFORMAT_ASTC_10x5,
        PIXELFORMAT_ASTC_10x6,
        PIXELFORMAT_ASTC_10x8,
        PIXELFORMAT_ASTC_10x10,
        PIXELFORMAT_ASTC_12x10,
        PIXELFORMAT_ASTC_12x12,

        PIXELFORMAT_MAX_ENUM
    };

    int GetPixelFormatColorComponents(PixelFormat format);

    unsigned GetPixelFormatSize(PixelFormat format);

    bool IsPixelFormatCompressed(PixelFormat format);
} // namespace love
