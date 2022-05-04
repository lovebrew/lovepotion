#pragma once

#include <stddef.h>

namespace love
{
    enum PixelFormat
    {
        /* standard */
        PIXELFORMAT_UNKNOWN,
        PIXELFORMAT_NORMAL,
        PIXELFORMAT_RGB8,
        PIXELFORMAT_RGBA8_UNORM,
        PIXELFORMAT_RGBA8_UNORM_SRGB,
        PIXELFORMAT_R8_UNORM,
        PIXELFORMAT_RGB565_UNORM,
        PIXELFORMAT_RGBA4_UNORM,
        PIXELFORMAT_RGBA16_UNORM,
        PIXELFORMAT_LA8_UNORM,
        /* depth/stencil */
        PIXELFORMAT_STENCIL8,
        PIXELFORMAT_DEPTH16_UNORM,
        PIXELFORMAT_DEPTH24_UNORM,
        PIXELFORMAT_DEPTH32_FLOAT,
        PIXELFORMAT_DEPTH24_UNORM_STENCIL8,
        PIXELFORMAT_DEPTH32_FLOAT_STENCIL8,
        /* compressed dxt */
        PIXELFORMAT_DXT1_UNORM,
        PIXELFORMAT_DXT3_UNORM,
        PIXELFORMAT_DXT5_UNORM,
        /* compressed bc4 */
        PIXELFORMAT_BC4_UNORM,
        PIXELFORMAT_BC4_SNORM,
        PIXELFORMAT_BC5_UNORM,
        PIXELFORMAT_BC5_SNORM,
        PIXELFORMAT_BC6H_UFLOAT,
        PIXELFORMAT_BC6H_FLOAT,
        PIXELFORMAT_BC7_UNORM,
        /* compressed pvr */
        PIXELFORMAT_PVR1_RGB2_UNORM,
        PIXELFORMAT_PVR1_RGB4_UNORM,
        PIXELFORMAT_PVR1_RGBA2_UNORM,
        PIXELFORMAT_PVR1_RGBA4_UNORM,
        /* compressed etc */
        PIXELFORMAT_ETC1_UNORM,
        PIXELFORMAT_ETC2_RGB_UNORM,
        PIXELFORMAT_ETC2_RGBA_UNORM,
        PIXELFORMAT_ETC2_RGBA1_UNORM,
        /* compressed eac */
        PIXELFORMAT_EAC_R_UNORM,
        PIXELFORMAT_EAC_R_SNORM,
        PIXELFORMAT_EAC_RG_UNORM,
        PIXELFORMAT_EAC_RG_SNORM,
        /* compressed astc */
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
        /* ------------ */
        PIXELFORMAT_MAX_ENUM
    };

    enum PixelFormatType
    {
        PIXELFORMAT_TYPE_UNORM,
        PIXELFORMAT_TYPE_SNORM,
        PIXELFORMAT_TYPE_UFLOAT,
        PIXELFORMAT_TYPE_SFLOAT,
        PIXELFORMAT_TYPE_UINT,
        PIXELFORMAT_TYPE_SINT
    };

    struct PixelFormatInfo
    {
        int components;
        size_t blockWidth;
        size_t blockHeight;
        size_t blockSize;
        bool color;
        bool depth;
        bool stencil;
        bool compressed;
        bool sRGB;
        PixelFormatType dataType;
    };

    bool GetPixelFormatConstant(const char* in, PixelFormat& out);

    bool GetPixelFormatConstant(PixelFormat in, const char*& out);

    const char* GetPixelFormatName(PixelFormat format);

    const PixelFormatInfo& GetPixelFormatInfo(PixelFormat format);

    bool IsPixelFormatCompressed(PixelFormat format);

    bool IsPixelFormatColor(PixelFormat format);

    bool IsPixelFormatDepthStencil(PixelFormat format);

    bool IsPixelFormatDepth(PixelFormat format);

    bool IsPixelFormatStencil(PixelFormat format);

    bool IsPixelFormatSRGB(PixelFormat format);

    bool IsPixelFormatInteger(PixelFormat format);

    PixelFormat GetSRGBPixelFormat(PixelFormat format);

    PixelFormat GetLinearPixelFormat(PixelFormat format);

    size_t GetPixelFormatBlockSize(PixelFormat format);

    size_t GetPixelFormatUncompressedRowSize(PixelFormat format, int width);

    size_t GetPixelFormatCompressedBlockRowSize(PixelFormat format, int width);

    size_t GetPixelFormatCompressedBlockRowCount(PixelFormat format, int height);

    size_t GetPixelFormatSliceSize(PixelFormat format, int width, int height);

    size_t GetPixelFormatColorComponents(PixelFormat format);
} // namespace love
