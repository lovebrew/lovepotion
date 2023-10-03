#pragma once

#pragma once

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>
#include <utilities/formathandler/formathandler.hpp>

namespace love
{
    class KTXHandler : public FormatHandler
    {
      public:
        enum KTXGLInternalFormat : uint32_t
        {
            /* ETC2 */
            KTX_GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2  = 0x9276,
            KTX_GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 = 0x9277,
            KTX_GL_COMPRESSED_RGBA8_ETC2_EAC                 = 0x9278,
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC          = 0x9279,

            /* DXT */
            KTX_GL_COMPRESSED_RGB_S3TC_DXT1_EXT        = 0x83F0,
            KTX_GL_COMPRESSED_RGBA_S3TC_DXT3_EXT       = 0x83F2,
            KTX_GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT = 0x8C4E,
            KTX_GL_COMPRESSED_RGBA_S3TC_DXT5_EXT       = 0x83F3,
            KTX_GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT = 0x8C4F,

            /* BC4 and BC5 */
            KTX_GL_COMPRESSED_RED_RGTC1 = 0x8DBB,
            KTX_GL_COMPRESSED_RG_RGTC2  = 0x8DBD,

            /* BC6 and BC7 */
            KTX_GL_COMPRESSED_RGBA_BPTC_UNORM       = 0x8E8C,
            KTX_GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM = 0x8E8D,

            /* ASTC */
            KTX_GL_COMPRESSED_RGBA_ASTC_4x4_KHR           = 0x93B0,
            KTX_GL_COMPRESSED_RGBA_ASTC_5x4_KHR           = 0x93B1,
            KTX_GL_COMPRESSED_RGBA_ASTC_5x5_KHR           = 0x93B2,
            KTX_GL_COMPRESSED_RGBA_ASTC_6x5_KHR           = 0x93B3,
            KTX_GL_COMPRESSED_RGBA_ASTC_6x6_KHR           = 0x93B4,
            KTX_GL_COMPRESSED_RGBA_ASTC_8x5_KHR           = 0x93B5,
            KTX_GL_COMPRESSED_RGBA_ASTC_8x6_KHR           = 0x93B6,
            KTX_GL_COMPRESSED_RGBA_ASTC_8x8_KHR           = 0x93B7,
            KTX_GL_COMPRESSED_RGBA_ASTC_10x5_KHR          = 0x93B8,
            KTX_GL_COMPRESSED_RGBA_ASTC_10x6_KHR          = 0x93B9,
            KTX_GL_COMPRESSED_RGBA_ASTC_10x8_KHR          = 0x93BA,
            KTX_GL_COMPRESSED_RGBA_ASTC_10x10_KHR         = 0x93BB,
            KTX_GL_COMPRESSED_RGBA_ASTC_12x10_KHR         = 0x93BC,
            KTX_GL_COMPRESSED_RGBA_ASTC_12x12_KHR         = 0x93BD,
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR   = 0x93D0,
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR   = 0x93D1,
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR   = 0x93D2,
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR   = 0x93D3,
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR   = 0x93D4,
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR   = 0x93D5,
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR   = 0x93D6,
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR   = 0x93D7,
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR  = 0x93D8,
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR  = 0x93D9,
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR  = 0x93DA,
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR = 0x93DB,
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR = 0x93DC,
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR = 0x93DD
        };

        virtual ~KTXHandler()
        {}

        bool CanParseCompressed(Data* data) override;

        StrongReference<ByteData> ParseCompressed(
            Data* filedata, std::vector<StrongReference<CompressedSlice>>& images,
            PixelFormat& format, bool& sRGB) override;

        struct ConvertedFormat
        {
            PixelFormat format;
            bool sRGB;

            bool operator==(const ConvertedFormat&) const = default;
        };

        // clang-format off
        static constexpr BidirectionalMap ktxFormats = 
        {
            KTX_GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,  ConvertedFormat { PIXELFORMAT_ETC2_RGBA1_UNORM, false },
            KTX_GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, ConvertedFormat { PIXELFORMAT_ETC2_RGBA1_UNORM, true  },
            KTX_GL_COMPRESSED_RGBA8_ETC2_EAC,                 ConvertedFormat { PIXELFORMAT_ETC2_RGBA_UNORM,  false },
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,          ConvertedFormat { PIXELFORMAT_ETC2_RGBA_UNORM,  true  },
            KTX_GL_COMPRESSED_RGB_S3TC_DXT1_EXT,              ConvertedFormat { PIXELFORMAT_DXT1_UNORM,       false },
            KTX_GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,             ConvertedFormat { PIXELFORMAT_DXT3_UNORM,       false },
            KTX_GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,       ConvertedFormat { PIXELFORMAT_DXT3_UNORM,       true  },
            KTX_GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,             ConvertedFormat { PIXELFORMAT_DXT5_UNORM,       false },
            KTX_GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,       ConvertedFormat { PIXELFORMAT_DXT5_UNORM,       true  },
            KTX_GL_COMPRESSED_RED_RGTC1,                      ConvertedFormat { PIXELFORMAT_BC4_UNORM,        false },
            KTX_GL_COMPRESSED_RG_RGTC2,                       ConvertedFormat { PIXELFORMAT_BC5_UNORM,        false },
            KTX_GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,          ConvertedFormat { PIXELFORMAT_BC7_UNORM,        true  },
            KTX_GL_COMPRESSED_RGBA_BPTC_UNORM,                ConvertedFormat { PIXELFORMAT_BC7_UNORM,        false },
            KTX_GL_COMPRESSED_RGBA_ASTC_4x4_KHR,              ConvertedFormat { PIXELFORMAT_ASTC_4x4,         false },
            KTX_GL_COMPRESSED_RGBA_ASTC_5x4_KHR,              ConvertedFormat { PIXELFORMAT_ASTC_5x4,         false },
            KTX_GL_COMPRESSED_RGBA_ASTC_5x5_KHR,              ConvertedFormat { PIXELFORMAT_ASTC_5x5,         false },
            KTX_GL_COMPRESSED_RGBA_ASTC_6x5_KHR,              ConvertedFormat { PIXELFORMAT_ASTC_6x5,         false },
            KTX_GL_COMPRESSED_RGBA_ASTC_6x6_KHR,              ConvertedFormat { PIXELFORMAT_ASTC_6x6,         false },
            KTX_GL_COMPRESSED_RGBA_ASTC_8x5_KHR,              ConvertedFormat { PIXELFORMAT_ASTC_8x5,         false },
            KTX_GL_COMPRESSED_RGBA_ASTC_8x6_KHR,              ConvertedFormat { PIXELFORMAT_ASTC_8x6,         false },
            KTX_GL_COMPRESSED_RGBA_ASTC_8x8_KHR,              ConvertedFormat { PIXELFORMAT_ASTC_8x8,         false },
            KTX_GL_COMPRESSED_RGBA_ASTC_10x5_KHR,             ConvertedFormat { PIXELFORMAT_ASTC_10x5,        false },
            KTX_GL_COMPRESSED_RGBA_ASTC_10x6_KHR,             ConvertedFormat { PIXELFORMAT_ASTC_10x6,        false },
            KTX_GL_COMPRESSED_RGBA_ASTC_10x8_KHR,             ConvertedFormat { PIXELFORMAT_ASTC_10x8,        false },
            KTX_GL_COMPRESSED_RGBA_ASTC_10x10_KHR,            ConvertedFormat { PIXELFORMAT_ASTC_10x10,       false },
            KTX_GL_COMPRESSED_RGBA_ASTC_12x10_KHR,            ConvertedFormat { PIXELFORMAT_ASTC_12x10,       false },
            KTX_GL_COMPRESSED_RGBA_ASTC_12x12_KHR,            ConvertedFormat { PIXELFORMAT_ASTC_12x12,       false },
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR,      ConvertedFormat { PIXELFORMAT_ASTC_4x4,         true  },
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR,      ConvertedFormat { PIXELFORMAT_ASTC_5x4,         true  },
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR,      ConvertedFormat { PIXELFORMAT_ASTC_5x5,         true  },
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR,      ConvertedFormat { PIXELFORMAT_ASTC_6x5,         true  },
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR,      ConvertedFormat { PIXELFORMAT_ASTC_6x6,         true  },
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR,      ConvertedFormat { PIXELFORMAT_ASTC_8x5,         true  },
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR,      ConvertedFormat { PIXELFORMAT_ASTC_8x6,         true  },
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR,      ConvertedFormat { PIXELFORMAT_ASTC_8x8,         true  },
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR,     ConvertedFormat { PIXELFORMAT_ASTC_10x5,        true  },
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR,     ConvertedFormat { PIXELFORMAT_ASTC_10x6,        true  },
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR,     ConvertedFormat { PIXELFORMAT_ASTC_10x8,        true  },
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR,    ConvertedFormat { PIXELFORMAT_ASTC_10x10,       true  },
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR,    ConvertedFormat { PIXELFORMAT_ASTC_12x10,       true  },
            KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR,    ConvertedFormat { PIXELFORMAT_ASTC_12x12,       true  }
        };
        // clang-format on

      private:
        struct KTXHeader
        {
            uint8_t identifier[12];
            uint32_t endianness;
            uint32_t glType;
            uint32_t glTypeSize;
            uint32_t glFormat;
            uint32_t glInternalFormat;
            uint32_t glBaseInternalFormat;
            uint32_t pixelWidth;
            uint32_t pixelHeight;
            uint32_t pixelDepth;
            uint32_t numberOfArrayElements;
            uint32_t numberOfFaces;
            uint32_t numberOfMipmapLevels;
            uint32_t bytesOfKeyValueData;
        };

        static constexpr uint8_t IDENTIFIER_REF[12] = { 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31,
                                                        0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A };

        static constexpr uint32_t KTX_ENDIAN_REF     = 0x04030201;
        static constexpr uint32_t KTX_ENDIAN_REF_REV = 0x01020304;
        static constexpr uint32_t HEADER_SIZE        = 0x40;
    };
} // namespace love