#include "common/Exception.hpp"
#include "common/int.hpp"

#include "modules/image/magpie/PKMHandler.hpp"

#include <cstring>

namespace love
{
    static constexpr uint8_t PKM_MAGIC[4] = { 'P', 'K', 'M', ' ' };

    struct PKMHeader
    {
        uint8_t identifier[4];
        uint8_t version[2];
        uint16_t textureFormatBig;
        uint16_t extendedWidthBig;
        uint16_t extendedHeightBig;
        uint16_t widthBig;
        uint16_t heightBig;
    };

    enum PKMTextureFormat
    {
        ETC1_RGB_NO_MIPMAPS = 0,
        ETC2PACKAGE_RGB_NO_MIPMAPS,
        ETC2PACKAGE_RGBA_NO_MIPMAPS_OLD,
        ETC2PACKAGE_RGBA_NO_MIPMAPS,
        ETC2PACKAGE_RGBA1_NO_MIPMAPS,
        ETC2PACKAGE_R_NO_MIPMAPS,
        ETC2PACKAGE_RG_NO_MIPMAPS,
        ETC2PACKAGE_R_SIGNED_NO_MIPMAPS,
        ETC2PACKAGE_RG_SIGNED_NO_MIPMAPS
    };

    static PixelFormat convertFormat(uint16_t texformat)
    {
        switch (texformat)
        {
            case ETC1_RGB_NO_MIPMAPS:
                return PIXELFORMAT_ETC1_UNORM;
            case ETC2PACKAGE_RGB_NO_MIPMAPS:
                return PIXELFORMAT_ETC2_RGB_UNORM;
            case ETC2PACKAGE_RGBA_NO_MIPMAPS_OLD:
            case ETC2PACKAGE_RGBA_NO_MIPMAPS:
                return PIXELFORMAT_ETC2_RGBA_UNORM;
            case ETC2PACKAGE_RGBA1_NO_MIPMAPS:
                return PIXELFORMAT_ETC2_RGBA1_UNORM;
            case ETC2PACKAGE_R_NO_MIPMAPS:
                return PIXELFORMAT_EAC_R_UNORM;
            case ETC2PACKAGE_RG_NO_MIPMAPS:
                return PIXELFORMAT_EAC_RG_UNORM;
            case ETC2PACKAGE_R_SIGNED_NO_MIPMAPS:
                return PIXELFORMAT_EAC_R_SNORM;
            case ETC2PACKAGE_RG_SIGNED_NO_MIPMAPS:
                return PIXELFORMAT_EAC_RG_SNORM;
            default:
                return PIXELFORMAT_UNKNOWN;
        }
    }

    bool PKMHandler::canParseCompressed(Data* data) const
    {
        if (data->getSize() <= sizeof(PKMHeader))
            return false;

        const auto* header = (const PKMHeader*)data->getData();

        if (std::memcmp(header->identifier, PKM_MAGIC, sizeof(PKM_MAGIC)) != 0)
            return false;

        if ((header->version[0] != '2' && header->version[0] != '1') || header->version[1] != '0')
            return false;

        return true;
    }

    StrongRef<ByteData> PKMHandler::parseCompressed(Data* filedata, CompressedSlices& images,
                                                    PixelFormat& format) const
    {
        if (!this->canParseCompressed(filedata))
            throw love::Exception("Could not decode compressed data (not a PKM file?)");

        auto header = *(const PKMHeader*)filedata->getData();

        header.textureFormatBig  = swap16_big(header.textureFormatBig);
        header.extendedWidthBig  = swap16_big(header.extendedWidthBig);
        header.extendedHeightBig = swap16_big(header.extendedHeightBig);
        header.widthBig          = swap16_big(header.widthBig);
        header.heightBig         = swap16_big(header.heightBig);

        auto convertedFormat = convertFormat(header.textureFormatBig);

        if (convertedFormat == PIXELFORMAT_UNKNOWN)
            throw love::Exception("Could not parse PKM file: unsupported texture format.");

        size_t totalSize = filedata->getSize() - sizeof(PKMHeader);

        StrongRef<ByteData> memory(new ByteData(totalSize, false), Acquire::NO_RETAIN);
        std::memcpy(memory->getData(), (uint8_t*)filedata->getData() + sizeof(PKMHeader), totalSize);

        int width  = header.widthBig;
        int height = header.heightBig;

        // clang-format off
        images.emplace_back(new CompressedSlice(convertedFormat, width, height, memory, 0, totalSize), Acquire::NO_RETAIN);
        // clang-format on

        format = convertedFormat;
        return memory;
    }
} // namespace love
