#pragma once

#include "common/data.h"
#include "common/pixelformat.h"

#include "objects/imagedata/types/compressedmemory.h"
#include "objects/object.h"

namespace love
{
    class FormatHandler : public Object
    {
        enum EncodedFormat
        {
            ENCODED_TGA,
            ENCODED_PNG,
            ENCODED_T3X,
            ENCODED_MAX_ENUM
        };

        struct DecodedImage
        {
            PixelFormat format = PIXELFORMAT_RGBA8;

            int width   = 0;
            int height  = 0;
            size_t size = 0;

            unsigned char* data = nullptr;
        };

        struct EncodedImage
        {
            size_t size         = 0;
            unsigned char* data = nullptr;
        };

        FormatHandler();

        virtual ~FormatHandler();

        virtual bool CanDecode(Data* data);

        virtual bool CanEncode(PixelFormat rawFormat, EncodedFormat encodedFormat);

        virtual DecodedImage Decode(Data* data);

        virtual EncodedImage Encode(const DecodedImage& image, EncodedFormat format);

        virtual bool CanParseCompressed(Data* data);

        virtual void FreeRawPixels(unsigned char* memory);
    };
} // namespace love
