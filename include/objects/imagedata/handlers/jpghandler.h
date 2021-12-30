#pragma once

#include "objects/imagedata/types/formathandler.h"

namespace love
{
    class JPGHandler : public FormatHandler
    {
      public:
        virtual bool CanDecode(Data* data);

        virtual bool CanEncode(PixelFormat rawFormat, EncodedFormat encodedFormat);

        virtual DecodedImage Decode(Data* data);

        virtual EncodedImage Encode(const DecodedImage& image, EncodedFormat format);

        virtual void FreeRawPixels(unsigned char* memory);
    };
} // namespace love
