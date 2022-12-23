#pragma once

#include <common/data.hpp>
#include <common/object.hpp>
#include <common/strongreference.hpp>

#include <common/pixelformat.hpp>

#include <objects/data/bytedata/bytedata.hpp>

namespace love
{
    class CompressedSlice;

    class FormatHandler : public Object
    {
      public:
        enum EncodedFormat
        {
            ENCODED_TGA,
            ENCODED_PNG,
            ENCODED_EXR
        };

        struct DecodedImage
        {
            PixelFormat format = PIXELFORMAT_RGBA8_UNORM;
            int width          = 0;
            int height         = 0;
            size_t size        = 0;
            std::unique_ptr<uint8_t[]> data;
        };

        struct EncodedImage
        {
            size_t size   = 0;
            uint8_t* data = nullptr;
        };

        virtual ~FormatHandler()
        {}

        virtual bool CanDecode(Data* data);

        virtual bool CanEncode(PixelFormat format, EncodedFormat encodedFormat);

        virtual DecodedImage Decode(Data* data);

        virtual EncodedImage Encode(const DecodedImage& image, EncodedFormat format);

        virtual bool CanParseCompressed(Data* data);

        virtual StrongReference<ByteData> ParseCompressed(
            Data* filedata, std::vector<StrongReference<CompressedSlice>>& images,
            PixelFormat& format, bool& sRGB);

        void FreeRawPixels(uint8_t* memory)
        {
            delete[] memory;
        }

        void FreeEncodedImage(uint8_t* memory)
        {
            delete[] memory;
        }
    };
} // namespace love
