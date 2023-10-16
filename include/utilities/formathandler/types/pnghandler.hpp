#pragma once

#include <utilities/formathandler/formathandler.hpp>

namespace love
{
    class PNGHandler : public FormatHandler
    {
      public:
        virtual bool CanDecode(Data* data);

        virtual DecodedImage Decode(Data* data);

        virtual bool CanEncode(PixelFormat rawFormat, EncodedFormat encodedFormat);

        virtual EncodedImage Encode(const DecodedImage& image, EncodedFormat format);
    };
} // namespace love
