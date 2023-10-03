#pragma once

#include <utilities/formathandler/formathandler.hpp>

namespace love
{
    class PKMHandler : public FormatHandler
    {
      public:
        static constexpr uint8_t identifier[] = { 'P', 'K', 'M', ' ' };

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
            ETC1_RGB_NO_MIPMAPS,
            ETC2PACKAGE_RGB_NO_MIPMAPS,
            ETC2PACKAGE_RGBA_NO_MIPMAPS_OLD,
            ETC2PACKAGE_RGBA_NO_MIPMAPS,
            ETC2PACKAGE_RGBA1_NO_MIPMAPS,
            ETC2PACKAGE_R_NO_MIPMAPS,
            ETC2PACKAGE_RG_NO_MIPMAPS,
            ETC2PACKAGE_R_SIGNED_NO_MIPMAPS,
            ETC2PACKAGE_RG_SIGNED_NO_MIPMAPS
        };

        virtual ~PKMHandler()
        {}

        // Implements FormatHandler.
        bool CanParseCompressed(Data* data) override;

        StrongReference<ByteData> ParseCompressed(
            Data* filedata, std::vector<StrongReference<CompressedSlice>>& images,
            PixelFormat& format, bool& sRGB) override;
    };
} // namespace love