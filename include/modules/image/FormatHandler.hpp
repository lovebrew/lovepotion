#pragma once

#include "common/Data.hpp"
#include "common/Object.hpp"
#include "common/pixelformat.hpp"

#include "modules/image/CompressedSlice.hpp"

#include <vector>

namespace love
{
    using CompressedSlices = std::vector<StrongRef<CompressedSlice>>;

    class FormatHandler : public Object
    {
      public:
        enum EncodedFormat
        {
            ENCODED_TGA,
            ENCODED_PNG,
            ENCODED_EXR,
            ENCODED_MAX_ENUM
        };

        struct DecodedImage
        {
            PixelFormat format = PIXELFORMAT_RGBA8_UNORM;
            int width          = 0;
            int height         = 0;
            size_t size        = 0;
            uint8_t* data      = nullptr;
        };

        struct EncodedImage
        {
            size_t size;
            uint8_t* data;
        };

        FormatHandler();

        virtual ~FormatHandler();

        virtual bool canDecode(Data* data) const;

        virtual bool canEncode(PixelFormat, EncodedFormat) const;

        virtual DecodedImage decode(Data*) const;

        virtual EncodedImage encode(const DecodedImage&, EncodedFormat) const;

        virtual bool canParseCompressed(Data*) const;

        virtual StrongRef<ByteData> parseCompressed(Data* filedata, CompressedSlices& images,
                                                    PixelFormat& format) const;

        virtual void freeRawPixels(uint8_t* memory);

        virtual void freeEncodedImage(uint8_t* memory);
    };
} // namespace love
