#pragma once

#include "common/pixelformat.hpp"
#include "modules/image/FormatHandler.hpp"

namespace love
{
    class STBHandler : public FormatHandler
    {
      public:
        virtual ~STBHandler()
        {}

        bool canDecode(Data* data) override;

        bool canEncode(PixelFormat rawFormat, EncodedFormat encodedFormat) override;

        DecodedImage decode(Data* data) override;

        EncodedImage encode(const DecodedImage& image, EncodedFormat encodedFormat) override;

        void freeRawPixels(uint8_t* memory) override;

        void freeEncodedImage(uint8_t* memory) override;

      private:
        static constexpr auto HEADER_LENGTH = 18;
        static constexpr auto BPP           = 4;
    };
} // namespace love
