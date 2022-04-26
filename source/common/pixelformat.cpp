#include <string.h>

#include "common/lmath.h"
#include "common/pixelformat.h"

namespace love
{
    // clang-format off
    static constexpr PixelFormatInfo formatInfo[] =
    {
        /* components, blockW, blockH, blockSize, color, depth, stencil, compressed, sRGB dataType */
        { 0, 1, 1, 0, false, false, false, false, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_UNKNOWN
        { 0, 1, 1, 0, true,  false, false, false, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_NORMAL
        { 3, 1, 1, 3, true,  false, false, false, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_RGB8
        { 4, 1, 1, 4, true,  false, false, false, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_RGBA8_UNORM
        { 4, 1, 1, 4, true,  false, false, false, true,  PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_RGBA8_UNORM_sRGB
        { 1, 1, 1, 1, true,  false, false, false, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_R8_UNORM
        { 3, 1, 1, 2, true,  false, false, false, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_RGB565_UNORM
        { 4, 1, 1, 2, true,  false, false, false, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_RGBA4_UNORM
        { 4, 1, 1, 8, true,  false, false, false, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_RGBA16_UNORM
        { 2, 1, 1, 2, true,  false, false, false, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_LA8_UNORM

        { 1, 1, 1, 1, false, false, true,  false, false, PIXELFORMAT_TYPE_UINT   }, // PIXELFORMAT_STENCIL8
        { 1, 1, 1, 2, false, true,  false, false, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_DEPTH16_UNORM
        { 1, 1, 1, 3, false, true,  false, false, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_DEPTH24_UNORM
        { 1, 1, 1, 4, false, true,  false, false, false, PIXELFORMAT_TYPE_SFLOAT }, // PIXELFORMAT_DEPTH32_FLOAT
        { 2, 1, 1, 4, false, true,  true , false, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_DEPTH24_UNORM_STENCIL8
        { 2, 1, 1, 5, false, true,  true , false, false, PIXELFORMAT_TYPE_SFLOAT }, // PIXELFORMAT_DEPTH32_FLOAT_STENCIL8

        { 3, 4, 4, 8,  true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_DXT1_UNORM
        { 4, 4, 4, 16, true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_DXT3_UNORM
        { 4, 4, 4, 16, true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_DXT5_UNORM

        { 1, 4, 4, 8,  true, false, false, true, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_BC4_UNORM
        { 1, 4, 4, 8,  true, false, false, true, false, PIXELFORMAT_TYPE_SNORM  }, // PIXELFORMAT_BC4_SNORM
        { 2, 4, 4, 16, true, false, false, true, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_BC5_UNORM
        { 2, 4, 4, 16, true, false, false, true, false, PIXELFORMAT_TYPE_SNORM  }, // PIXELFORMAT_BC5_SNORM
        { 3, 4, 4, 16, true, false, false, true, false, PIXELFORMAT_TYPE_UFLOAT }, // PIXELFORMAT_BC6H_UFLOAT
        { 3, 4, 4, 16, true, false, false, true, false, PIXELFORMAT_TYPE_SFLOAT }, // PIXELFORMAT_BC6H_FLOAT
        { 4, 4, 4, 16, true, false, false, true, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_BC7_UNORM

        { 3, 16, 8, 32, true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_PVR1_RGB2_UNORM
        { 3, 8,  8, 32, true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_PVR1_RGB4_UNORM
        { 4, 16, 8, 32, true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_PVR1_RGBA2_UNORM
        { 4, 8,  8, 32, true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_PVR1_RGBA4_UNORM

        { 3, 4, 4, 8,  true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_ETC1_UNORM
        { 3, 4, 4, 8,  true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_ETC2_RGB_UNORM
        { 4, 4, 4, 16, true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_ETC2_RGBA_UNORM
        { 4, 4, 4, 8,  true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_ETC2_RGBA1_UNORM
        { 1, 4, 4, 8,  true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_EAC_R_UNORM
        { 1, 4, 4, 8,  true, false, false, true, false, PIXELFORMAT_TYPE_SNORM }, // PIXELFORMAT_EAC_R_SNORM
        { 2, 4, 4, 16, true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_EAC_RG_UNORM
        { 2, 4, 4, 16, true, false, false, true, false, PIXELFORMAT_TYPE_SNORM }, // PIXELFORMAT_EAC_RG_SNORM

        { 4, 4,  4,  1, true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_ASTC_4x4
        { 4, 5,  4,  1, true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_ASTC_5x4
        { 4, 5,  5,  1, true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_ASTC_5x5
        { 4, 6,  5,  1, true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_ASTC_6x5
        { 4, 6,  6,  1, true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_ASTC_6x6
        { 4, 8,  5,  1, true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_ASTC_8x5
        { 4, 8,  6,  1, true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_ASTC_8x6
        { 4, 8,  8,  1, true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_ASTC_8x8
        { 4, 8,  5,  1, true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_ASTC_10x5
        { 4, 10, 6,  1, true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_ASTC_10x6
        { 4, 10, 8,  1, true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_ASTC_10x8
        { 4, 10, 10, 1, true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_ASTC_10x10
        { 4, 12, 10, 1, true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_ASTC_12x10
        { 4, 12, 12, 1, true, false, false, true, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_ASTC_12x12
    };
    // clang-format on

    static_assert(sizeof(formatInfo) / sizeof(PixelFormatInfo) == PIXELFORMAT_MAX_ENUM,
                  "Update the formatInfo array when adding or removing a PixelFormat");

    const PixelFormatInfo& GetPixelFormatInfo(PixelFormat format)
    {
        return formatInfo[format];
    }

    bool IsPixelFormatCompressed(PixelFormat format)
    {
        return formatInfo[format].compressed;
    }

    bool IsPixelFormatColor(PixelFormat format)
    {
        return formatInfo[format].color;
    }

    bool IsPixelFormatDepthStencil(PixelFormat format)
    {
        const PixelFormatInfo& info = formatInfo[format];

        return info.depth || info.stencil;
    }

    bool IsPixelFormatDepth(PixelFormat format)
    {
        return formatInfo[format].depth;
    }

    bool IsPixelFormatStencil(PixelFormat format)
    {
        return formatInfo[format].stencil;
    }

    bool IsPixelFormatSRGB(PixelFormat format)
    {
        return formatInfo[format].sRGB;
    }

    bool IsPixelFormatInteger(PixelFormat format)
    {
        /* TODO */
        return false;
    }

    PixelFormat GetSRGBPixelFormat(PixelFormat format)
    {
        if (format == PIXELFORMAT_RGBA8_UNORM)
            return PIXELFORMAT_RGBA8_UNORM_SRGB;

        return PIXELFORMAT_UNKNOWN;
    }

    PixelFormat GetLinearPixelFormat(PixelFormat format)
    {
        if (format == PIXELFORMAT_RGBA8_UNORM_SRGB)
            return PIXELFORMAT_RGBA8_UNORM;

        return PIXELFORMAT_UNKNOWN;
    }

    size_t GetPixelFormatBlockSize(PixelFormat format)
    {
        return formatInfo[format].blockSize;
    }

    size_t GetPixelFormatUncompressedRowSize(PixelFormat format, int width)
    {
        const PixelFormatInfo& info = formatInfo[format];

        if (info.compressed)
            return 0;

        return info.blockSize * width / info.blockWidth;
    }

    size_t GetPixelFormatCompressedBlockRowSize(PixelFormat format, int width)
    {
        const PixelFormatInfo& info = formatInfo[format];

        if (!info.compressed)
            return 0;

        return info.blockSize * ((width + info.blockWidth - 1)) / info.blockWidth;
    }

    size_t GetPixelFormatCompressedBlockRowCount(PixelFormat format, int height)
    {
        const PixelFormatInfo& info = formatInfo[format];

        if (!info.compressed)
            return 0;

        return (height + info.blockHeight - 1) / info.blockHeight;
    }

    size_t GetPixelFormatSliceSize(PixelFormat format, int width, int height)
    {
        const PixelFormatInfo& info = formatInfo[format];

        if (strncmp(__CONSOLE__, "3DS", 3) == 0)
        {
            width  = NextPo2(width + 2);
            height = NextPo2(height + 2);
        }

        size_t blockWidth  = (width + info.blockWidth - 1) / info.blockWidth;
        size_t blockHeight = (height + info.blockHeight - 1) / info.blockHeight;

        return info.blockSize * blockWidth * blockHeight;
    }

    size_t GetPixelFormatColorComponents(PixelFormat format)
    {
        return formatInfo[format].components;
    }
} // namespace love
