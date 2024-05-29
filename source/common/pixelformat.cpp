
#include "common/Console.hpp"
#include "common/Map.hpp"
#include "common/math.hpp"

#include "common/pixelformat.hpp"

namespace love
{
    // clang-format off
    static PixelFormatInfo formatInfo[] =
    {
        // components, blockW, blockH, blockSize, color, depth, stencil, compressed, sRGB, dataType
        { 0, 1, 1, 0, false, false, false, false, false, PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_UNKNOWN

        { 0, 1, 1, 0, true, false, false, false, false, PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_NORMAL
        { 0, 1, 1, 0, true, false, false, false, false, PIXELFORMATTYPE_SFLOAT }, // PIXELFORMAT_HDR

        { 1, 1, 1, 1, true, false, false, false, false, PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_R8_UNORM
        { 1, 1, 1, 1, true, false, false, false, false, PIXELFORMATTYPE_SINT   }, // PIXELFORMAT_R8_INT
        { 1, 1, 1, 1, true, false, false, false, false, PIXELFORMATTYPE_UINT   }, // PIXELFORMAT_R8_UINT
        { 1, 1, 1, 2, true, false, false, false, false, PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_R16_UNORM
        { 1, 1, 1, 2, true, false, false, false, false, PIXELFORMATTYPE_SFLOAT }, // PIXELFORMAT_R16_FLOAT
        { 1, 1, 1, 2, true, false, false, false, false, PIXELFORMATTYPE_SINT   }, // PIXELFORMAT_R16_INT
        { 1, 1, 1, 2, true, false, false, false, false, PIXELFORMATTYPE_UINT   }, // PIXELFORMAT_R16_UINT
        { 1, 1, 1, 4, true, false, false, false, false, PIXELFORMATTYPE_SFLOAT }, // PIXELFORMAT_R32_FLOAT
        { 1, 1, 1, 4, true, false, false, false, false, PIXELFORMATTYPE_SINT   }, // PIXELFORMAT_R32_INT
        { 1, 1, 1, 4, true, false, false, false, false, PIXELFORMATTYPE_UINT   }, // PIXELFORMAT_R32_UINT

        { 2, 1, 1, 2, true, false, false, false, false, PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_RG8_UNORM
        { 2, 1, 1, 2, true, false, false, false, false, PIXELFORMATTYPE_SINT   }, // PIXELFORMAT_RG8_INT
        { 2, 1, 1, 2, true, false, false, false, false, PIXELFORMATTYPE_UINT   }, // PIXELFORMAT_RG8_UINT
        { 2, 1, 1, 1, true, false, false, false, false, PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_A4_UNORM
        { 2, 1, 1, 2, true, false, false, false, false, PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_LA8_UNORM
        { 2, 1, 1, 4, true, false, false, false, false, PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_RG16_UNORM
        { 2, 1, 1, 4, true, false, false, false, false, PIXELFORMATTYPE_SFLOAT }, // PIXELFORMAT_RG16_FLOAT
        { 2, 1, 1, 4, true, false, false, false, false, PIXELFORMATTYPE_SINT   }, // PIXELFORMAT_RG16_INT
        { 2, 1, 1, 4, true, false, false, false, false, PIXELFORMATTYPE_UINT   }, // PIXELFORMAT_RG16_UINT
        { 2, 1, 1, 8, true, false, false, false, false, PIXELFORMATTYPE_SFLOAT }, // PIXELFORMAT_RG32_FLOAT
        { 2, 1, 1, 8, true, false, false, false, false, PIXELFORMATTYPE_SINT   }, // PIXELFORMAT_RG32_INT
        { 2, 1, 1, 8, true, false, false, false, false, PIXELFORMATTYPE_UINT   }, // PIXELFORMAT_RG32_UINT

        { 4, 1, 1, 4,  true, false, false, false, false, PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_RGBA8_UNORM
        { 4, 1, 1, 4,  true, false, false, false, true,  PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_RGBA8_sRGB
        { 4, 1, 1, 4,  true, false, false, false, false, PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_BGRA8_UNORM
        { 4, 1, 1, 4,  true, false, false, false, true,  PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_BGRA8_sRGB
        { 4, 1, 1, 4,  true, false, false, false, false, PIXELFORMATTYPE_SINT   }, // PIXELFORMAT_RGBA8_INT
        { 4, 1, 1, 4,  true, false, false, false, false, PIXELFORMATTYPE_UINT   }, // PIXELFORMAT_RGBA8_UINT
        { 4, 1, 1, 8,  true, false, false, false, false, PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_RGBA16_UNORM
        { 4, 1, 1, 8,  true, false, false, false, false, PIXELFORMATTYPE_SFLOAT }, // PIXELFORMAT_RGBA16_FLOAT
        { 4, 1, 1, 8,  true, false, false, false, false, PIXELFORMATTYPE_SINT   }, // PIXELFORMAT_RGBA16_INT
        { 4, 1, 1, 8,  true, false, false, false, false, PIXELFORMATTYPE_UINT   }, // PIXELFORMAT_RGBA16_UINT
        { 4, 1, 1, 16, true, false, false, false, false, PIXELFORMATTYPE_SFLOAT }, // PIXELFORMAT_RGBA32_FLOAT
        { 4, 1, 1, 16, true, false, false, false, false, PIXELFORMATTYPE_SINT   }, // PIXELFORMAT_RGBA32_INT
        { 4, 1, 1, 16, true, false, false, false, false, PIXELFORMATTYPE_UINT   }, // PIXELFORMAT_RGBA32_UINT

        { 4, 1, 1, 2, true, false, false, false, false, PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_RGBA4_UNORM
        { 4, 1, 1, 2, true, false, false, false, false, PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_RGB5A1_UNORM
        { 3, 1, 1, 2, true, false, false, false, false, PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_RGB565_UNORM
        { 4, 1, 1, 4, true, false, false, false, false, PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_RGB10A2_UNORM
        { 3, 1, 1, 4, true, false, false, false, false, PIXELFORMATTYPE_UFLOAT }, // PIXELFORMAT_RG11B10_FLOAT

        { 1, 1, 1, 1, false, false, true , false, false, PIXELFORMATTYPE_UINT   }, // PIXELFORMAT_STENCIL8
        { 1, 1, 1, 2, false, true,  false, false, false, PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_DEPTH16_UNORM
        { 1, 1, 1, 3, false, true,  false, false, false, PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_DEPTH24_UNORM
        { 1, 1, 1, 4, false, true,  false, false, false, PIXELFORMATTYPE_SFLOAT }, // PIXELFORMAT_DEPTH32_FLOAT
        { 2, 1, 1, 4, false, true,  true , false, false, PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_DEPTH24_UNORM_STENCIL8
        { 2, 1, 1, 5, false, true,  true , false, false, PIXELFORMATTYPE_SFLOAT }, // PIXELFORMAT_DEPTH32_FLOAT_STENCIL8

        { 3, 4, 4, 8,  true, false, false, true, false, PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_DXT1_UNORM
        { 3, 4, 4, 8,  true, false, false, true, true,  PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_DXT1_sRGB
        { 4, 4, 4, 16, true, false, false, true, false, PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_DXT3_UNORM
        { 4, 4, 4, 16, true, false, false, true, true,  PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_DXT3_sRGB
        { 4, 4, 4, 16, true, false, false, true, false, PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_DXT5_UNORM
        { 4, 4, 4, 16, true, false, false, true, true,  PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_DXT5_sRGB
        { 1, 4, 4, 8,  true, false, false, true, false, PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_BC4_UNORM
        { 1, 4, 4, 8,  true, false, false, true, false, PIXELFORMATTYPE_SNORM  }, // PIXELFORMAT_BC4_SNORM
        { 2, 4, 4, 16, true, false, false, true, false, PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_BC5_UNORM
        { 2, 4, 4, 16, true, false, false, true, false, PIXELFORMATTYPE_SNORM  }, // PIXELFORMAT_BC5_SNORM
        { 3, 4, 4, 16, true, false, false, true, false, PIXELFORMATTYPE_UFLOAT }, // PIXELFORMAT_BC6H_UFLOAT
        { 3, 4, 4, 16, true, false, false, true, false, PIXELFORMATTYPE_SFLOAT }, // PIXELFORMAT_BC6H_FLOAT
        { 4, 4, 4, 16, true, false, false, true, false, PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_BC7_UNORM
        { 4, 4, 4, 16, true, false, false, true, true,  PIXELFORMATTYPE_UNORM  }, // PIXELFORMAT_BC7_sRGB

        { 3, 16, 8, 32, true, false, false, true, false, PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_PVR1_RGB2_UNORM
        { 3, 16, 8, 32, true, false, false, true, true,  PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_PVR1_RGB2_sRGB
        { 3, 8,  8, 32, true, false, false, true, false, PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_PVR1_RGB4_UNORM
        { 3, 8,  8, 32, true, false, false, true, true,  PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_PVR1_RGB4_sRGB
        { 4, 16, 8, 32, true, false, false, true, false, PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_PVR1_RGBA2_UNORM
        { 4, 16, 8, 32, true, false, false, true, true,  PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_PVR1_RGBA2_sRGB
        { 4, 8,  8, 32, true, false, false, true, false, PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_PVR1_RGBA4_UNORM
        { 4, 8,  8, 32, true, false, false, true, true,  PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_PVR1_RGBA4_sRGB

        { 3, 4, 4, 8,  true, false, false, true, false, PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ETC1_UNORM
        { 3, 4, 4, 8,  true, false, false, true, false, PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ETC2_RGB_UNORM
        { 3, 4, 4, 8,  true, false, false, true, true,  PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ETC2_RGB_sRGB
        { 4, 4, 4, 16, true, false, false, true, false, PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ETC2_RGBA_UNORM
        { 4, 4, 4, 16, true, false, false, true, true,  PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ETC2_RGBA_sRGB
        { 4, 4, 4, 8,  true, false, false, true, false, PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ETC2_RGBA1_UNORM
        { 4, 4, 4, 8,  true, false, false, true, true,  PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ETC2_RGBA1_sRGB
        { 1, 4, 4, 8,  true, false, false, true, false, PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_EAC_R_UNORM
        { 1, 4, 4, 8,  true, false, false, true, false, PIXELFORMATTYPE_SNORM }, // PIXELFORMAT_EAC_R_SNORM
        { 2, 4, 4, 16, true, false, false, true, false, PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_EAC_RG_UNORM
        { 2, 4, 4, 16, true, false, false, true, false, PIXELFORMATTYPE_SNORM }, // PIXELFORMAT_EAC_RG_SNORM

        { 4, 4,  4,  1, true, false, false, true, false, PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_4x4_UNORM
        { 4, 5,  4,  1, true, false, false, true, false, PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_5x4_UNORM
        { 4, 5,  5,  1, true, false, false, true, false, PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_5x5_UNORM
        { 4, 6,  5,  1, true, false, false, true, false, PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_6x5_UNORM
        { 4, 6,  6,  1, true, false, false, true, false, PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_6x6_UNORM
        { 4, 8,  5,  1, true, false, false, true, false, PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_8x5_UNORM
        { 4, 8,  6,  1, true, false, false, true, false, PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_8x6_UNORM
        { 4, 8,  8,  1, true, false, false, true, false, PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_8x8_UNORM
        { 4, 8,  5,  1, true, false, false, true, false, PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_10x5_UNORM
        { 4, 10, 6,  1, true, false, false, true, false, PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_10x6_UNORM
        { 4, 10, 8,  1, true, false, false, true, false, PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_10x8_UNORM
        { 4, 10, 10, 1, true, false, false, true, false, PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_10x10_UNORM
        { 4, 12, 10, 1, true, false, false, true, false, PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_12x10_UNORM
        { 4, 12, 12, 1, true, false, false, true, false, PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_12x12_UNORM
        { 4, 4,  4,  1, true, false, false, true, true,  PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_4x4_sRGB
        { 4, 5,  4,  1, true, false, false, true, true,  PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_5x4_sRGB
        { 4, 5,  5,  1, true, false, false, true, true,  PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_5x5_sRGB
        { 4, 6,  5,  1, true, false, false, true, true,  PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_6x5_sRGB
        { 4, 6,  6,  1, true, false, false, true, true,  PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_6x6_sRGB
        { 4, 8,  5,  1, true, false, false, true, true,  PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_8x5_sRGB
        { 4, 8,  6,  1, true, false, false, true, true,  PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_8x6_sRGB
        { 4, 8,  8,  1, true, false, false, true, true,  PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_8x8_sRGB
        { 4, 8,  5,  1, true, false, false, true, true,  PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_10x5_sRGB
        { 4, 10, 6,  1, true, false, false, true, true,  PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_10x6_sRGB
        { 4, 10, 8,  1, true, false, false, true, true,  PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_10x8_sRGB
        { 4, 10, 10, 1, true, false, false, true, true,  PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_10x10_sRGB
        { 4, 12, 10, 1, true, false, false, true, true,  PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_12x10_sRGB
        { 4, 12, 12, 1, true, false, false, true, true,  PIXELFORMATTYPE_UNORM }, // PIXELFORMAT_ASTC_12x12_sRGB
    };

    static constexpr size_t FORMATS_SIZE = sizeof(formatInfo);
    static constexpr size_t FORMAT_STRUCT_SIZE = sizeof(PixelFormatInfo);

    static_assert(FORMATS_SIZE / FORMAT_STRUCT_SIZE == PIXELFORMAT_MAX_ENUM, "Update the formatInfo array when adding or removing a PixelFormat");
    static_assert(PixelFormats.size() == (size_t)PIXELFORMAT_MAX_ENUM, "Pixel format string map is missing entries!");
    // clang-format on

    const PixelFormatInfo& getPixelFormatInfo(PixelFormat format)
    {
        return formatInfo[format];
    }

    std::string_view getPixelFormatName(PixelFormat format)
    {
        std::string_view name = "unknown";
        getConstant(format, name);

        return name;
    }

    bool isPixelFormatCompressed(PixelFormat format)
    {
        return formatInfo[format].compressed;
    }

    bool isPixelFormatColor(PixelFormat format)
    {
        return formatInfo[format].color;
    }

    bool isPixelFormatDepthStencil(PixelFormat format)
    {
        const PixelFormatInfo& info = formatInfo[format];
        return info.depth || info.stencil;
    }

    bool isPixelFormatDepth(PixelFormat format)
    {
        return formatInfo[format].depth;
    }

    bool isPixelFormatStencil(PixelFormat format)
    {
        return formatInfo[format].stencil;
    }

    bool isPixelFormatSRGB(PixelFormat format)
    {
        return formatInfo[format].sRGB;
    }

    bool isPixelFormatInteger(PixelFormat format)
    {
        auto type = formatInfo[format].dataType;
        return type == PIXELFORMATTYPE_SINT || type == PIXELFORMATTYPE_UINT;
    }

    PixelFormat getSRGBPixelFormat(PixelFormat format)
    {
        switch (format)
        {
            case PIXELFORMAT_RGBA8_UNORM:
                return PIXELFORMAT_RGBA8_sRGB;
            case PIXELFORMAT_BGRA8_UNORM:
                return PIXELFORMAT_BGRA8_sRGB;
            case PIXELFORMAT_DXT1_UNORM:
                return PIXELFORMAT_DXT1_sRGB;
            case PIXELFORMAT_DXT3_UNORM:
                return PIXELFORMAT_DXT3_sRGB;
            case PIXELFORMAT_DXT5_UNORM:
                return PIXELFORMAT_DXT5_sRGB;
            case PIXELFORMAT_BC7_UNORM:
                return PIXELFORMAT_BC7_sRGB;
            case PIXELFORMAT_PVR1_RGB2_UNORM:
                return PIXELFORMAT_PVR1_RGB2_sRGB;
            case PIXELFORMAT_PVR1_RGB4_UNORM:
                return PIXELFORMAT_PVR1_RGB4_sRGB;
            case PIXELFORMAT_PVR1_RGBA2_UNORM:
                return PIXELFORMAT_PVR1_RGBA2_sRGB;
            case PIXELFORMAT_PVR1_RGBA4_UNORM:
                return PIXELFORMAT_PVR1_RGBA4_sRGB;
            case PIXELFORMAT_ETC1_UNORM:
                return PIXELFORMAT_ETC2_RGB_sRGB; // ETC2 can load ETC1 data.
            case PIXELFORMAT_ETC2_RGB_UNORM:
                return PIXELFORMAT_ETC2_RGB_sRGB;
            case PIXELFORMAT_ETC2_RGBA_UNORM:
                return PIXELFORMAT_ETC2_RGBA_sRGB;
            case PIXELFORMAT_ETC2_RGBA1_UNORM:
                return PIXELFORMAT_ETC2_RGBA1_sRGB;
            case PIXELFORMAT_ASTC_4x4_UNORM:
                return PIXELFORMAT_ASTC_4x4_sRGB;
            case PIXELFORMAT_ASTC_5x4_UNORM:
                return PIXELFORMAT_ASTC_5x4_sRGB;
            case PIXELFORMAT_ASTC_5x5_UNORM:
                return PIXELFORMAT_ASTC_5x5_sRGB;
            case PIXELFORMAT_ASTC_6x5_UNORM:
                return PIXELFORMAT_ASTC_6x5_sRGB;
            case PIXELFORMAT_ASTC_6x6_UNORM:
                return PIXELFORMAT_ASTC_6x6_sRGB;
            case PIXELFORMAT_ASTC_8x5_UNORM:
                return PIXELFORMAT_ASTC_8x5_sRGB;
            case PIXELFORMAT_ASTC_8x6_UNORM:
                return PIXELFORMAT_ASTC_8x6_sRGB;
            case PIXELFORMAT_ASTC_8x8_UNORM:
                return PIXELFORMAT_ASTC_8x8_sRGB;
            case PIXELFORMAT_ASTC_10x5_UNORM:
                return PIXELFORMAT_ASTC_10x5_sRGB;
            case PIXELFORMAT_ASTC_10x6_UNORM:
                return PIXELFORMAT_ASTC_10x6_sRGB;
            case PIXELFORMAT_ASTC_10x8_UNORM:
                return PIXELFORMAT_ASTC_10x8_sRGB;
            case PIXELFORMAT_ASTC_10x10_UNORM:
                return PIXELFORMAT_ASTC_10x10_sRGB;
            case PIXELFORMAT_ASTC_12x10_UNORM:
                return PIXELFORMAT_ASTC_12x10_sRGB;
            case PIXELFORMAT_ASTC_12x12_UNORM:
                return PIXELFORMAT_ASTC_12x12_sRGB;
            default:
                return format;
        }
    }

    PixelFormat getLinearPixelFormat(PixelFormat format)
    {
        switch (format)
        {
            case PIXELFORMAT_RGBA8_sRGB:
                return PIXELFORMAT_RGBA8_UNORM;
            case PIXELFORMAT_BGRA8_sRGB:
                return PIXELFORMAT_BGRA8_UNORM;
            case PIXELFORMAT_DXT1_sRGB:
                return PIXELFORMAT_DXT1_UNORM;
            case PIXELFORMAT_DXT3_sRGB:
                return PIXELFORMAT_DXT3_UNORM;
            case PIXELFORMAT_DXT5_sRGB:
                return PIXELFORMAT_DXT5_UNORM;
            case PIXELFORMAT_BC7_sRGB:
                return PIXELFORMAT_BC7_UNORM;
            case PIXELFORMAT_PVR1_RGB2_sRGB:
                return PIXELFORMAT_PVR1_RGB2_UNORM;
            case PIXELFORMAT_PVR1_RGB4_sRGB:
                return PIXELFORMAT_PVR1_RGB4_UNORM;
            case PIXELFORMAT_PVR1_RGBA2_sRGB:
                return PIXELFORMAT_PVR1_RGBA2_UNORM;
            case PIXELFORMAT_PVR1_RGBA4_sRGB:
                return PIXELFORMAT_PVR1_RGBA4_UNORM;
            case PIXELFORMAT_ETC2_RGB_sRGB:
                return PIXELFORMAT_ETC2_RGB_UNORM;
            case PIXELFORMAT_ETC2_RGBA_sRGB:
                return PIXELFORMAT_ETC2_RGBA_UNORM;
            case PIXELFORMAT_ETC2_RGBA1_sRGB:
                return PIXELFORMAT_ETC2_RGBA1_UNORM;
            case PIXELFORMAT_ASTC_4x4_sRGB:
                return PIXELFORMAT_ASTC_4x4_UNORM;
            case PIXELFORMAT_ASTC_5x4_sRGB:
                return PIXELFORMAT_ASTC_5x4_UNORM;
            case PIXELFORMAT_ASTC_5x5_sRGB:
                return PIXELFORMAT_ASTC_5x5_UNORM;
            case PIXELFORMAT_ASTC_6x5_sRGB:
                return PIXELFORMAT_ASTC_6x5_UNORM;
            case PIXELFORMAT_ASTC_6x6_sRGB:
                return PIXELFORMAT_ASTC_6x6_UNORM;
            case PIXELFORMAT_ASTC_8x5_sRGB:
                return PIXELFORMAT_ASTC_8x5_UNORM;
            case PIXELFORMAT_ASTC_8x6_sRGB:
                return PIXELFORMAT_ASTC_8x6_UNORM;
            case PIXELFORMAT_ASTC_8x8_sRGB:
                return PIXELFORMAT_ASTC_8x8_UNORM;
            case PIXELFORMAT_ASTC_10x5_sRGB:
                return PIXELFORMAT_ASTC_10x5_UNORM;
            case PIXELFORMAT_ASTC_10x6_sRGB:
                return PIXELFORMAT_ASTC_10x6_UNORM;
            case PIXELFORMAT_ASTC_10x8_sRGB:
                return PIXELFORMAT_ASTC_10x8_UNORM;
            case PIXELFORMAT_ASTC_10x10_sRGB:
                return PIXELFORMAT_ASTC_10x10_UNORM;
            case PIXELFORMAT_ASTC_12x10_sRGB:
                return PIXELFORMAT_ASTC_12x10_UNORM;
            case PIXELFORMAT_ASTC_12x12_sRGB:
                return PIXELFORMAT_ASTC_12x12_UNORM;
            default:
                return format;
        }
    }

    size_t getPixelFormatBlockSize(PixelFormat format)
    {
        return formatInfo[format].blockSize;
    }

    size_t getPixelFormatUncompressedRowSize(PixelFormat format, int width)
    {
        const PixelFormatInfo& info = formatInfo[format];

        if (info.compressed)
            return 0;

        return info.blockSize * width / info.blockWidth;
    }

    size_t getPixelFormatCompressedBlockRowSize(PixelFormat format, int width)
    {
        const PixelFormatInfo& info = formatInfo[format];

        if (!info.compressed)
            return 0;

        return info.blockSize * ((width + info.blockWidth - 1) / info.blockWidth);
    }

    size_t getPixelFormatCompressedBlockRowCount(PixelFormat format, int height)
    {
        const PixelFormatInfo& info = formatInfo[format];

        if (!info.compressed)
            return 0;

        return (height + info.blockHeight - 1) / info.blockHeight;
    }

    size_t getPixelFormatSliceSize(PixelFormat format, int width, int height, bool needsPo2)
    {
        const PixelFormatInfo& info = formatInfo[format];

        if (Console::is(Console::CTR) && needsPo2)
        {
            width  = NextPo2(width);
            height = NextPo2(height);
        }

        size_t blockW = (width + info.blockWidth - 1) / info.blockWidth;
        size_t blockH = (height + info.blockHeight - 1) / info.blockHeight;

        return info.blockSize * blockW * blockH;
    }

    int getPixelFormatColorComponents(PixelFormat format)
    {
        return formatInfo[format].components;
    }
} // namespace love
