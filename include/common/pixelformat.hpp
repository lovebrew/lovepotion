#pragma once

#include <stddef.h>

#include "utilities/bidirectionalmap/bidirectionalmap.hpp"

namespace love
{
    enum PixelFormat
    {
        /* standard */
        PIXELFORMAT_UNKNOWN,

        PIXELFORMAT_NORMAL,
        PIXELFORMAT_HDR,

        /* single channel */
        PIXELFORMAT_R8_UNORM,
        PIXELFORMAT_R8_INT,
        PIXELFORMAT_R8_UINT,
        PIXELFORMAT_R16_UNORM,
        PIXELFORMAT_R16_FLOAT,
        PIXELFORMAT_R16_INT,
        PIXELFORMAT_R16_UINT,
        PIXELFORMAT_R32_FLOAT,
        PIXELFORMAT_R32_INT,
        PIXELFORMAT_R32_UINT,

        /* dual channel */
        PIXELFORMAT_RG8_UNORM,
        PIXELFORMAT_RG8_INT,
        PIXELFORMAT_RG8_UINT,
        PIXELFORMAT_LA8_UNORM,
        PIXELFORMAT_RG16_UNORM,
        PIXELFORMAT_RG16_FLOAT,
        PIXELFORMAT_RG16_INT,
        PIXELFORMAT_RG16_UINT,
        PIXELFORMAT_RG32_FLOAT,
        PIXELFORMAT_RG32_INT,
        PIXELFORMAT_RG32_UINT,

        /* triple channel */
        PIXELFORMAT_RGB8,

        /* quad channel */
        PIXELFORMAT_RGBA8_UNORM,
        PIXELFORMAT_RGBA8_UNORM_SRGB,
        PIXELFROMAT_BGRA8_UNORM,
        PIXELFORMAT_BGRA8_UNORM_SRGB,
        PIXELFORMAT_RGBA8_INT,
        PIXELFORMAT_RGBA8_UINT,
        PIXELFORMAT_RGBA16_UNORM,
        PIXELFORMAT_RGBA16_FLOAT,
        PIXELFORMAT_RGBA16_INT,
        PIXELFORMAT_RGBA16_UINT,
        PIXELFORMAT_RGBA32_FLOAT,
        PIXELFORMAT_RGBA32_INT,
        PIXELFORMAT_RGBA32_UINT,

        /* packed formats */
        PIXELFORMAT_RGBA4_UNORM,
        PIXELFORMAT_RGB5A1_UNORM,
        PIXELFORMAT_RGB565_UNORM,
        PIXELFORMAT_RGB10A2_UNORM,
        PIXELFORMAT_RG11B10_FLOAT,

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
        PIXELFORMAT_BC6H_SFLOAT,
        PIXELFORMAT_BC7_UNORM,
        PIXELFORMAT_BC7_UNORM_SRGB,

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

    enum PixelFormatUsage
    {
        PIXELFORMAT_USAGE_SAMPLE,       // Any sampling in shaders.
        PIXELFORMAT_USAGE_LINEAR,       // Linear filtering.
        PIXELFORMAT_USAGE_RENDERTARGET, // Usable as a render target.
        PIXELFORMAT_USAGE_BLEND,        // Blend support when used as a render target.
        PIXELFORMAT_USAGE_MSAA,         // MSAA support when used as a render target.
        PIXELFORMAT_USAGE_COMPUTEWRITE, // Writable in compute shaders via imageStore.
        PIXELFORMAT_USAGE_MAX_ENUM
    };

    enum PixelFormatUsageFlags
    {
        PIXELFORMAT_USAGE_FLAGS_NONE         = 0,
        PIXELFORMAT_USAGE_FLAGS_SAMPLE       = (1 << PIXELFORMAT_USAGE_SAMPLE),
        PIXELFORMAT_USAGE_FLAGS_LINEAR       = (1 << PIXELFORMAT_USAGE_LINEAR),
        PIXELFORMAT_USAGE_FLAGS_RENDERTARGET = (1 << PIXELFORMAT_USAGE_RENDERTARGET),
        PIXELFORMAT_USAGE_FLAGS_BLEND        = (1 << PIXELFORMAT_USAGE_BLEND),
        PIXELFORMAT_USAGE_FLAGS_MSAA         = (1 << PIXELFORMAT_USAGE_MSAA),
        PIXELFORMAT_USAGE_FLAGS_COMPUTEWRITE = (1 << PIXELFORMAT_USAGE_COMPUTEWRITE),
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

    size_t GetPixelFormatSliceSize(PixelFormat format, int width, int height,
                                   bool needPowerOfTwo = true);

    size_t GetPixelFormatColorComponents(PixelFormat format);

    // clang-format off
    static constexpr BidirectionalMap pixelFormats = {
        "unknown",         PIXELFORMAT_UNKNOWN,

        "normal",          PIXELFORMAT_NORMAL,
        "r8",              PIXELFORMAT_R8_UNORM,
        "rgb565",          PIXELFORMAT_RGB565_UNORM,
        "rgba8",           PIXELFORMAT_RGBA8_UNORM,
        "rgba16",          PIXELFORMAT_RGBA16_UNORM,

        "stencil8",        PIXELFORMAT_STENCIL8,
        "depth16",         PIXELFORMAT_DEPTH16_UNORM,
        "depth24",         PIXELFORMAT_DEPTH24_UNORM,
        "depth32f",        PIXELFORMAT_DEPTH32_FLOAT,
        "depth24stencil8", PIXELFORMAT_DEPTH24_UNORM_STENCIL8,

        "DXT1",            PIXELFORMAT_DXT1_UNORM,
        "DXT3",            PIXELFORMAT_DXT3_UNORM,
        "DXT5",            PIXELFORMAT_DXT5_UNORM,

        "BC4",             PIXELFORMAT_BC4_UNORM,
        "BC4s",            PIXELFORMAT_BC4_SNORM,
        "BC5",             PIXELFORMAT_BC5_UNORM,
        "BC5s",            PIXELFORMAT_BC5_SNORM,
        "BC6h",            PIXELFORMAT_BC6H_UFLOAT,
        "BC6hs",           PIXELFORMAT_BC6H_SFLOAT,
        "BC7",             PIXELFORMAT_BC7_UNORM,

        "PVR1rgb2",        PIXELFORMAT_PVR1_RGB2_UNORM,
        "PVR1rgb4",        PIXELFORMAT_PVR1_RGB4_UNORM,
        "PVR1rgba2",       PIXELFORMAT_PVR1_RGBA2_UNORM,
        "PVR1rgba4",       PIXELFORMAT_PVR1_RGBA4_UNORM,

        "ETC1",            PIXELFORMAT_ETC1_UNORM,
        "ETC2rgb",         PIXELFORMAT_ETC2_RGB_UNORM,
        "ETC2rgba",        PIXELFORMAT_ETC2_RGBA_UNORM,
        "ETC2rgba1",       PIXELFORMAT_ETC2_RGBA1_UNORM,
        "EACr",            PIXELFORMAT_EAC_R_UNORM,
        "EACrs",           PIXELFORMAT_EAC_R_SNORM,
        "EACrg",           PIXELFORMAT_EAC_RG_UNORM,
        "EACrgs",          PIXELFORMAT_EAC_RG_SNORM,

        "ASTC4x4",         PIXELFORMAT_ASTC_4x4,
        "ASTC5x4",         PIXELFORMAT_ASTC_5x4,
        "ASTC5x5",         PIXELFORMAT_ASTC_5x5,
        "ASTC6x5",         PIXELFORMAT_ASTC_6x5,
        "ASTC6x6",         PIXELFORMAT_ASTC_6x6,
        "ASTC8x5",         PIXELFORMAT_ASTC_8x5,
        "ASTC8x6",         PIXELFORMAT_ASTC_8x6,
        "ASTC8x8",         PIXELFORMAT_ASTC_8x8,
        "ASTC10x5",        PIXELFORMAT_ASTC_10x5,
        "ASTC10x6",        PIXELFORMAT_ASTC_10x6,
        "ASTC10x8",        PIXELFORMAT_ASTC_10x8,
        "ASTC10x10",       PIXELFORMAT_ASTC_10x10,
        "ASTC12x10",       PIXELFORMAT_ASTC_12x10,
        "ASTC12x12",       PIXELFORMAT_ASTC_12x12
    };
    // clang-format on
} // namespace love
