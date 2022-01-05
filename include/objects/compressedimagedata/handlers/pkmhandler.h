#pragma once

#include "objects/imagedata/types/formathandler.h"

namespace love
{
    class PKMHandler : public FormatHandler
    {
      public:
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

        bool CanParseCompressed(Data* data) override;

        StrongReference<CompressedMemory> ParseCompressed(
            Data* fileData, std::vector<StrongReference<CompressedSlice>>& images,
            PixelFormat& format, bool& isSRGB) override;

        const char* GetName() override
        {
            return "PKMHandler";
        }

      private:
        static constexpr uint8_t PKMIDENTIFIER[] = { 'P', 'K', 'M', ' ' };

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
    };
}; // namespace love
