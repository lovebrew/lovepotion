#pragma once

#include "modules/image/FormatHandler.hpp"

namespace love
{
    class QOIHandler : public FormatHandler
    {
      public:
        virtual ~QOIHandler()
        {}

        bool canDecode(Data* data) override;

        bool canEncode(PixelFormat rawFormat, EncodedFormat encodedFormat) const override;

        DecodedImage decode(Data*) override;

        EncodedImage encode(const DecodedImage& decodedImage, EncodedFormat encodedFormat) override;

        void freeRawPixels(uint8_t* memory) override;

        void freeEncodedImage(uint8_t* memory) override;

      private:
        struct Header
        {
            char magic[4];
            uint32_t width;
            uint32_t height;
            uint8_t channels;
            uint8_t colorSpace;
        };

        static constexpr auto MAGIC = "qoif";
    };
} // namespace love
