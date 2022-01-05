#pragma once

#include "common/data.h"
#include "common/pixelformat.h"

#include "objects/compressedimagedata/types/compressedmemory.h"
#include "objects/compressedimagedata/types/compressedslice.h"

#include "objects/object.h"

#include <vector>

namespace love
{
    class FormatHandler : public Object
    {
      public:
        enum EncodedFormat
        {
            ENCODED_TGA,
            ENCODED_PNG,
            ENCODED_MAX_ENUM
        };

        struct DecodedImage
        {
            PixelFormat format = PIXELFORMAT_RGBA8;

            int width   = 0;
            int height  = 0;
            size_t size = 0;

#if defined(__3DS__)
            int subWidth  = 0;
            int subHeight = 0;
#endif

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

        virtual const char* GetName()
        {
            return "FormatHandler";
        }

        virtual bool CanParseCompressed(Data* data);

        virtual StrongReference<CompressedMemory> ParseCompressed(
            Data* filedata, std::vector<StrongReference<CompressedSlice>>& images,
            PixelFormat& format, bool& sRGB);

        virtual void FreeRawPixels(unsigned char* memory);
    };
} // namespace love
