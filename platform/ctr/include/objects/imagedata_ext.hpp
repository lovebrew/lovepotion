#pragma once

#include <objects/imagedata/imagedata.tcc>

#include <common/math.hpp>
#include <common/strongreference.hpp>

#include <utilities/formathandler/types/t3xhandler.hpp>

namespace love
{
    template<>
    class ImageData<Console::CTR> : public ImageData<Console::ALL>
    {
      public:
        ImageData(Data* data);

        ImageData(int width, int height, PixelFormat format = PIXELFORMAT_RGBA8_UNORM) :
            ImageData<Console::ALL>(width, height, format)
        {
            if (!this->ValidPixelFormat(format))
            {
                throw love::Exception("ImageData does not support the %s pixel format",
                                      love::GetPixelFormatName(format));
            }

            this->Create(width, height, format);
        }

        ImageData(int width, int height, PixelFormat format, void* data, bool own) :
            ImageData<Console::ALL>(width, height, format)
        {
            if (!this->ValidPixelFormat(format))
            {
                throw love::Exception("ImageData does not support the %s pixel format",
                                      love::GetPixelFormatName(format));
            }

            if (own)
                this->data.reset((uint8_t*)data);
            else
                this->Create(width, height, format, data);
        }

        ImageData(const ImageData& other) :
            ImageData<Console::ALL>(other.width, other.height, other.format)
        {
            this->Create(width, height, format, other.GetData());
        }

        virtual ~ImageData();

        ImageData* Clone() const override
        {
            return new ImageData(*this);
        }

        void Paste(ImageData* data, int x, int y, Rect& source);

        void SetPixel(int x, int y, const Color& color);

        void GetPixel(int x, int y, Color& color);

        Color GetPixel(int x, int y);

        FileData* Encode(FormatHandler::EncodedFormat format, std::string_view filename,
                         bool writeFile) const;

      private:
        void Create(int width, int height, PixelFormat format, void* data = nullptr);

        void Decode(Data* data);

        StrongReference<FormatHandler> decoder;
    };
} // namespace love
