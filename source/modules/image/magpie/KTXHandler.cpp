#include "common/Exception.hpp"

#include "modules/image/magpie/KTXHandler.hpp"

#include <cstring>

namespace love
{
#define KTX_IDENTIFIER_REF                                                     \
    {                                                                          \
        0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A \
    }
#define KTX_ENDIAN_REF     (0x04030201)
#define KTX_ENDIAN_REF_REV (0x01020304)
#define KTX_HEADER_SIZE    (64)

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

    static_assert(sizeof(KTXHeader) == KTX_HEADER_SIZE, "Real size of KTX header doesn't match struct size!");

    enum KTXGLInternalFormat
    {
        KTX_GL_ETC1_RGB8_OES = 0x8D64,

        // ETC2 and EAC.
        KTX_GL_COMPRESSED_R11_EAC                        = 0x9270,
        KTX_GL_COMPRESSED_SIGNED_R11_EAC                 = 0x9271,
        KTX_GL_COMPRESSED_RG11_EAC                       = 0x9272,
        KTX_GL_COMPRESSED_SIGNED_RG11_EAC                = 0x9273,
        KTX_GL_COMPRESSED_RGB8_ETC2                      = 0x9274,
        KTX_GL_COMPRESSED_SRGB8_ETC2                     = 0x9275,
        KTX_GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2  = 0x9276,
        KTX_GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 = 0x9277,
        KTX_GL_COMPRESSED_RGBA8_ETC2_EAC                 = 0x9278,
        KTX_GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC          = 0x9279,

        // PVRTC1.
        KTX_GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG  = 0x8C00,
        KTX_GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG  = 0x8C01,
        KTX_GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG = 0x8C02,
        KTX_GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG = 0x8C03,

        // DXT1, DXT3, and DXT5.
        KTX_GL_COMPRESSED_RGB_S3TC_DXT1_EXT        = 0x83F0,
        KTX_GL_COMPRESSED_RGBA_S3TC_DXT3_EXT       = 0x83F2,
        KTX_GL_COMPRESSED_RGBA_S3TC_DXT5_EXT       = 0x83F3,
        KTX_GL_COMPRESSED_SRGB_S3TC_DXT1_EXT       = 0x8C4C,
        KTX_GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT = 0x8C4E,
        KTX_GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT = 0x8C4F,

        // BC4 and BC5.
        KTX_GL_COMPRESSED_RED_RGTC1        = 0x8DBB,
        KTX_GL_COMPRESSED_SIGNED_RED_RGTC1 = 0x8DBC,
        KTX_GL_COMPRESSED_RG_RGTC2         = 0x8DBD,
        KTX_GL_COMPRESSED_SIGNED_RG_RGTC2  = 0x8DBE,

        // BC6 and BC7.
        KTX_GL_COMPRESSED_RGBA_BPTC_UNORM         = 0x8E8C,
        KTX_GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM   = 0x8E8D,
        KTX_GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT   = 0x8E8E,
        KTX_GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT = 0x8E8F,

        // ASTC.
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

    PixelFormat convertFormat(uint32_t glformat)
    {
        // hnnngg ASTC...

        switch (glformat)
        {
            case KTX_GL_ETC1_RGB8_OES:
                return PIXELFORMAT_ETC1_UNORM;

            // EAC and ETC2.
            case KTX_GL_COMPRESSED_R11_EAC:
                return PIXELFORMAT_EAC_R_UNORM;
            case KTX_GL_COMPRESSED_SIGNED_R11_EAC:
                return PIXELFORMAT_EAC_R_SNORM;
            case KTX_GL_COMPRESSED_RG11_EAC:
                return PIXELFORMAT_EAC_RG_UNORM;
            case KTX_GL_COMPRESSED_SIGNED_RG11_EAC:
                return PIXELFORMAT_EAC_RG_SNORM;
            case KTX_GL_COMPRESSED_RGB8_ETC2:
                return PIXELFORMAT_ETC2_RGB_UNORM;
            case KTX_GL_COMPRESSED_SRGB8_ETC2:
                return PIXELFORMAT_ETC2_RGB_sRGB;
            case KTX_GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
                return PIXELFORMAT_ETC2_RGBA1_UNORM;
            case KTX_GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
                return PIXELFORMAT_ETC2_RGBA1_sRGB;
            case KTX_GL_COMPRESSED_RGBA8_ETC2_EAC:
                return PIXELFORMAT_ETC2_RGBA_UNORM;
            case KTX_GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
                return PIXELFORMAT_ETC2_RGBA_sRGB;

            // PVRTC.
            case KTX_GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
                return PIXELFORMAT_PVR1_RGB4_UNORM;
            case KTX_GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
                return PIXELFORMAT_PVR1_RGB2_UNORM;
            case KTX_GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
                return PIXELFORMAT_PVR1_RGBA4_UNORM;
            case KTX_GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG:
                return PIXELFORMAT_PVR1_RGBA2_UNORM;

            // DXT.
            case KTX_GL_COMPRESSED_SRGB_S3TC_DXT1_EXT:
                return PIXELFORMAT_DXT1_sRGB;
            case KTX_GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
                return PIXELFORMAT_DXT1_UNORM;
            case KTX_GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:
                return PIXELFORMAT_DXT3_sRGB;
            case KTX_GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
                return PIXELFORMAT_DXT3_UNORM;
            case KTX_GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
                return PIXELFORMAT_DXT5_sRGB;
            case KTX_GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
                return PIXELFORMAT_DXT5_UNORM;

            // BC4 and BC5.
            case KTX_GL_COMPRESSED_RED_RGTC1:
                return PIXELFORMAT_BC4_UNORM;
            case KTX_GL_COMPRESSED_SIGNED_RED_RGTC1:
                return PIXELFORMAT_BC4_SNORM;
            case KTX_GL_COMPRESSED_RG_RGTC2:
                return PIXELFORMAT_BC5_UNORM;
            case KTX_GL_COMPRESSED_SIGNED_RG_RGTC2:
                return PIXELFORMAT_BC5_SNORM;

            // BC6 and BC7.
            case KTX_GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM:
                return PIXELFORMAT_BC7_sRGB;
            case KTX_GL_COMPRESSED_RGBA_BPTC_UNORM:
                return PIXELFORMAT_BC7_UNORM;
            case KTX_GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT:
                return PIXELFORMAT_BC6H_FLOAT;
            case KTX_GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT:
                return PIXELFORMAT_BC6H_UFLOAT;

            // ASTC.
            case KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
                return PIXELFORMAT_ASTC_4x4_sRGB;
            case KTX_GL_COMPRESSED_RGBA_ASTC_4x4_KHR:
                return PIXELFORMAT_ASTC_4x4_UNORM;
            case KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
                return PIXELFORMAT_ASTC_5x4_sRGB;
            case KTX_GL_COMPRESSED_RGBA_ASTC_5x4_KHR:
                return PIXELFORMAT_ASTC_5x4_UNORM;
            case KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
                return PIXELFORMAT_ASTC_5x5_sRGB;
            case KTX_GL_COMPRESSED_RGBA_ASTC_5x5_KHR:
                return PIXELFORMAT_ASTC_5x5_UNORM;
            case KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
                return PIXELFORMAT_ASTC_6x5_sRGB;
            case KTX_GL_COMPRESSED_RGBA_ASTC_6x5_KHR:
                return PIXELFORMAT_ASTC_6x5_UNORM;
            case KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
                return PIXELFORMAT_ASTC_6x6_sRGB;
            case KTX_GL_COMPRESSED_RGBA_ASTC_6x6_KHR:
                return PIXELFORMAT_ASTC_6x6_UNORM;
            case KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
                return PIXELFORMAT_ASTC_8x5_sRGB;
            case KTX_GL_COMPRESSED_RGBA_ASTC_8x5_KHR:
                return PIXELFORMAT_ASTC_8x5_UNORM;
            case KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
                return PIXELFORMAT_ASTC_8x6_sRGB;
            case KTX_GL_COMPRESSED_RGBA_ASTC_8x6_KHR:
                return PIXELFORMAT_ASTC_8x6_UNORM;
            case KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
                return PIXELFORMAT_ASTC_8x8_sRGB;
            case KTX_GL_COMPRESSED_RGBA_ASTC_8x8_KHR:
                return PIXELFORMAT_ASTC_8x8_UNORM;
            case KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
                return PIXELFORMAT_ASTC_10x5_sRGB;
            case KTX_GL_COMPRESSED_RGBA_ASTC_10x5_KHR:
                return PIXELFORMAT_ASTC_10x5_UNORM;
            case KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
                return PIXELFORMAT_ASTC_10x6_sRGB;
            case KTX_GL_COMPRESSED_RGBA_ASTC_10x6_KHR:
                return PIXELFORMAT_ASTC_10x6_UNORM;
            case KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
                return PIXELFORMAT_ASTC_10x8_sRGB;
            case KTX_GL_COMPRESSED_RGBA_ASTC_10x8_KHR:
                return PIXELFORMAT_ASTC_10x8_UNORM;
            case KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
                return PIXELFORMAT_ASTC_10x10_sRGB;
            case KTX_GL_COMPRESSED_RGBA_ASTC_10x10_KHR:
                return PIXELFORMAT_ASTC_10x10_UNORM;
            case KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
                return PIXELFORMAT_ASTC_12x10_sRGB;
            case KTX_GL_COMPRESSED_RGBA_ASTC_12x10_KHR:
                return PIXELFORMAT_ASTC_12x10_UNORM;
            case KTX_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
                return PIXELFORMAT_ASTC_12x12_sRGB;
            case KTX_GL_COMPRESSED_RGBA_ASTC_12x12_KHR:
                return PIXELFORMAT_ASTC_12x12_UNORM;
            default:
                return PIXELFORMAT_UNKNOWN;
        }
    }

    bool KTXHandler::canParseCompressed(Data* data) const
    {
        if (data->getSize() < KTX_HEADER_SIZE)
            return false;

        auto* header           = (KTXHeader*)data->getData();
        uint8_t identifier[12] = KTX_IDENTIFIER_REF;

        if (memcmp(header->identifier, identifier, sizeof(identifier)) != 0)
            return false;

        if (header->endianness != KTX_ENDIAN_REF && header->endianness != KTX_ENDIAN_REF_REV)
            return false;

        return true;
    }

    StrongRef<ByteData> KTXHandler::parseCompressed(Data* filedata, CompressedSlices& images,
                                                    PixelFormat& format) const
    {
        if (!this->canParseCompressed(filedata))
            throw love::Exception("Could not decode compressed data (not a KTX file?)");

        auto header = *(KTXHeader*)filedata->getData();

        if (header.endianness == KTX_ENDIAN_REF_REV)
        {
            uint32_t* array = (uint32_t*)&header.glType;

            for (int i = 0; i < 12; i++)
                array[i] = swap_uint32(array[i]);
        }

        header.numberOfMipmapLevels = std::max<size_t>(header.numberOfMipmapLevels, 1);
        auto convertedFormat        = convertFormat(header.glInternalFormat);

        if (convertedFormat == PIXELFORMAT_UNKNOWN)
            throw love::Exception("Unsupported image format in KTX file.");

        if (header.numberOfArrayElements > 0)
            throw love::Exception("Texture arrays in KTX files are not supported.");

        if (header.pixelDepth > 1)
            throw love::Exception("3D textures in KTX files are not supported.");

        if (header.numberOfFaces > 1)
            throw love::Exception("Cubemap textures in KTX files are not supported.");

        size_t offset        = sizeof(KTXHeader) + header.bytesOfKeyValueData;
        const uint8_t* bytes = (const uint8_t*)filedata->getData();
        size_t totalSize     = 0;

        for (int index = 0; index < (int)header.numberOfMipmapLevels; index++)
        {
            if (offset + sizeof(uint32_t) > filedata->getSize())
                throw love::Exception("Could not parse KTX file: unexpected EOF.");

            uint32_t mipmapSize = *(const uint32_t*)(bytes + offset);

            if (header.endianness == KTX_ENDIAN_REF_REV)
                mipmapSize = swap_uint32(mipmapSize);

            offset += sizeof(uint32_t);

            uint32_t mipmapSizePadded = (mipmapSize + 3) & ~uint32_t(3);

            totalSize += mipmapSizePadded;
            offset += mipmapSizePadded;
        }

        StrongRef<ByteData> memory(new ByteData(totalSize, false), Acquire::NO_RETAIN);

        offset            = sizeof(KTXHeader) + header.bytesOfKeyValueData;
        size_t dataOffset = 0;

        for (int index = 0; index < (int)header.numberOfMipmapLevels; index++)
        {
            uint32_t mipmapSize = *(const uint32_t*)(bytes + offset);

            if (header.endianness == KTX_ENDIAN_REF_REV)
                mipmapSize = swap_uint32(mipmapSize);

            offset += sizeof(uint32_t);

            uint32_t mipmapSizePadded = (mipmapSize + 3) & ~uint32_t(3);

            int width  = std::max<int>(header.pixelWidth >> index, 1);
            int height = std::max<int>(header.pixelHeight >> index, 1);

            std::memcpy((uint8_t*)memory->getData() + dataOffset, bytes + offset, mipmapSize);

            auto* slice = new CompressedSlice(convertedFormat, width, height, memory, dataOffset, mipmapSize);
            images.push_back(slice);
            slice->release();

            offset += mipmapSizePadded;
            dataOffset += mipmapSizePadded;
        }

        format = convertedFormat;
        return memory;
    }
} // namespace love
