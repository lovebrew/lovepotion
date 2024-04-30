#pragma once

#include "modules/image/FormatHandler.hpp"

namespace love
{
    class PNGHandler : public FormatHandler
    {
      public:
        virtual ~PNGHandler()
        {}

        bool canDecode(Data* data) const override;

        bool canEncode(PixelFormat rawFormat, EncodedFormat encodedFormat) const override;

        DecodedImage decode(Data* data) const override;

        EncodedImage encode(const DecodedImage& image, EncodedFormat format) const override;
    };
} // namespace love
