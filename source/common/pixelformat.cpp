#include <string.h>

#include <common/console.hpp>
#include <common/math.hpp>
#include <common/pixelformat.hpp>

namespace love
{
    // clang-format off
    static constexpr PixelFormatInfo formatInfo[] =
    {
        /* components, blockW, blockH, blockSize, color, depth, stencil, compressed, sRGB dataType */
        { 0, 1, 1, 0, false, false, false, false, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_UNKNOWN

        { 0, 1, 1, 0, true,  false, false, false, false, PIXELFORMAT_TYPE_UNORM }, // PIXELFORMAT_NORMAL

        { 1, 1, 1, 1, true, false, false, false, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_R8_UNORM
        { 1, 1, 1, 1, true, false, false, false, false, PIXELFORMAT_TYPE_SINT   }, // PIXELFORMAT_R8_INT
        { 1, 1, 1, 1, true, false, false, false, false, PIXELFORMAT_TYPE_UINT   }, // PIXELFORMAT_R8_UINT
        { 1, 1, 1, 2, true, false, false, false, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_R16_UNORM
        { 1, 1, 1, 2, true, false, false, false, false, PIXELFORMAT_TYPE_SFLOAT }, // PIXELFORMAT_R16_FLOAT
        { 1, 1, 1, 2, true, false, false, false, false, PIXELFORMAT_TYPE_SINT   }, // PIXELFORMAT_R16_INT
        { 1, 1, 1, 2, true, false, false, false, false, PIXELFORMAT_TYPE_UINT   }, // PIXELFORMAT_R16_UINT
        { 1, 1, 1, 4, true, false, false, false, false, PIXELFORMAT_TYPE_SFLOAT }, // PIXELFORMAT_R32_FLOAT
        { 1, 1, 1, 4, true, false, false, false, false, PIXELFORMAT_TYPE_SINT   }, // PIXELFORMAT_R32_INT
        { 1, 1, 1, 4, true, false, false, false, false, PIXELFORMAT_TYPE_UINT   }, // PIXELFORMAT_R32_UINT

        { 2, 1, 1, 2, true, false, false, false, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_RG8_UNORM
        { 2, 1, 1, 2, true, false, false, false, false, PIXELFORMAT_TYPE_SINT   }, // PIXELFORMAT_RG8_INT
        { 2, 1, 1, 2, true, false, false, false, false, PIXELFORMAT_TYPE_UINT   }, // PIXELFORMAT_RG8_UINT
        { 2, 1, 1, 2, true, false, false, false, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_LA8_UNORM
        { 2, 1, 1, 4, true, false, false, false, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_RG16_UNORM
        { 2, 1, 1, 4, true, false, false, false, false, PIXELFORMAT_TYPE_SFLOAT }, // PIXELFORMAT_RG16_FLOAT
        { 2, 1, 1, 4, true, false, false, false, false, PIXELFORMAT_TYPE_SINT   }, // PIXELFORMAT_RG16_INT
        { 2, 1, 1, 4, true, false, false, false, false, PIXELFORMAT_TYPE_UINT   }, // PIXELFORMAT_RG16_UINT
        { 2, 1, 1, 8, true, false, false, false, false, PIXELFORMAT_TYPE_SFLOAT }, // PIXELFORMAT_RG32_FLOAT
        { 2, 1, 1, 8, true, false, false, false, false, PIXELFORMAT_TYPE_SINT   }, // PIXELFORMAT_RG32_INT
        { 2, 1, 1, 8, true, false, false, false, false, PIXELFORMAT_TYPE_UINT   }, // PIXELFORMAT_RG32_UINT

        { 3, 1, 1, 3, true,  false, false, false, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_RGB8

        { 4, 1, 1, 4,  true, false, false, false, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_RGBA8_UNORM
        { 4, 1, 1, 4,  true, false, false, false, true,  PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_RGBA8_UNORM_sRGB
        { 4, 1, 1, 4,  true, false, false, false, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_BGRA8_UNORM
        { 4, 1, 1, 4,  true, false, false, false, true,  PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_BGRA8_UNORM_sRGB
        { 4, 1, 1, 4,  true, false, false, false, false, PIXELFORMAT_TYPE_SINT   }, // PIXELFORMAT_RGBA8_INT
        { 4, 1, 1, 4,  true, false, false, false, false, PIXELFORMAT_TYPE_UINT   }, // PIXELFORMAT_RGBA8_UINT
        { 4, 1, 1, 8,  true, false, false, false, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_RGBA16_UNORM
        { 4, 1, 1, 8,  true, false, false, false, false, PIXELFORMAT_TYPE_SFLOAT }, // PIXELFORMAT_RGBA16_FLOAT
        { 4, 1, 1, 8,  true, false, false, false, false, PIXELFORMAT_TYPE_SINT   }, // PIXELFORMAT_RGBA16_INT
        { 4, 1, 1, 8,  true, false, false, false, false, PIXELFORMAT_TYPE_UINT   }, // PIXELFORMAT_RGBA16_UINT
        { 4, 1, 1, 16, true, false, false, false, false, PIXELFORMAT_TYPE_SFLOAT }, // PIXELFORMAT_RGBA32_FLOAT
        { 4, 1, 1, 16, true, false, false, false, false, PIXELFORMAT_TYPE_SINT   }, // PIXELFORMAT_RGBA32_INT
        { 4, 1, 1, 16, true, false, false, false, false, PIXELFORMAT_TYPE_UINT   }, // PIXELFORMAT_RGBA32_UINT

        { 4, 1, 1, 2, true, false, false, false, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_RGBA4_UNORM
        { 4, 1, 1, 2, true, false, false, false, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_RGB5A1_UNORM
        { 3, 1, 1, 2, true, false, false, false, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_RGB565_UNORM
        { 4, 1, 1, 4, true, false, false, false, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_RGB10A2_UNORM
        { 3, 1, 1, 4, true, false, false, false, false, PIXELFORMAT_TYPE_UFLOAT }, // PIXELFORMAT_RG11B10_FLOAT

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
        { 3, 4, 4, 16, true, false, false, true, false, PIXELFORMAT_TYPE_SFLOAT }, // PIXELFORMAT_BC6H_SFLOAT
        { 4, 4, 4, 16, true, false, false, true, false, PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_BC7_UNORM
        { 4, 4, 4, 16, true, false, false, true, true,  PIXELFORMAT_TYPE_UNORM  }, // PIXELFORMAT_BC7_UNORM_SRGB

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

    static constexpr int FORMATS_LIST_SIZE = sizeof(formatInfo) / sizeof(PixelFormatInfo);
    static_assert(FORMATS_LIST_SIZE == PIXELFORMAT_MAX_ENUM,
                  "Update the formatInfo array when adding or removing a PixelFormat.");

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

        if (Console::Is(Console::CTR))
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

    const char* GetPixelFormatName(PixelFormat format)
    {
        std::optional<const char*> name;
        if (!(name = pixelFormats.ReverseFind(format)))
            return *pixelFormats.ReverseFind(PIXELFORMAT_UNKNOWN);

        return *name;
    }
} // namespace love
