#pragma once

#include <common/color.hpp>
#include <common/console.hpp>
#include <common/data.hpp>
#include <common/exception.hpp>
#include <common/pixelformat.hpp>

#include <objects/data/filedata/filedata.hpp>

#include <utilities/threads/threads.hpp>

#include <functional>
#include <memory>

namespace love
{
    template<Console::Platform T = Console::ALL>
    class ImageData : public Data
    {
      public:
        union Pixel
        {
            uint8_t rgba8[4];
            uint16_t rgba16[4];
            // float16 rgba16f[4];
            float rgba32f[4];
            uint16_t packed16;
            uint32_t packed32;
        };

        static inline Type type = Type("ImageData", &Object::type);

        using PixelSetFunction = std::function<void(const Color& color, Pixel* pixel)>;
        using PixelGetFunction = std::function<void(const Pixel* pixel, Color& color)>;

        ImageData(int width, int height, PixelFormat format = PIXELFORMAT_RGBA8_UNORM) :
            ImageData(format, width, height)
        {
            if (!this->ValidPixelFormat(format))
            {
                throw love::Exception("ImageData does not support the %s pixel format",
                                      love::GetPixelFormatName(format));
            }

            this->Create(width, height, format);
            this->data = std::make_unique<uint8_t[]>(this->GetSize());
        }

        ImageData(int width, int height, PixelFormat format, void* data, bool own) :
            ImageData(format, width, height)
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

        ImageData(const ImageData& other) : ImageData(other.format, other.width, other.height)
        {
            this->Create(width, height, format, other.GetData());
        }

        virtual ~ImageData()
        {}

        void Create(int width, int height, PixelFormat format, void* data = nullptr)
        {
            const auto size = GetPixelFormatSliceSize(format, width, height);

            try
            {
                this->data = std::make_unique<uint8_t[]>(size);
            }
            catch (std::bad_alloc&)
            {
                throw love::Exception("Out of memory.");
            }

            if (data)
                std::copy_n((uint8_t*)data, size, this->data.get());
        }

        PixelFormat GetFormat() const
        {
            return this->format;
        }

        int GetWidth() const
        {
            return this->width;
        }

        int GetHeight() const
        {
            return this->height;
        }

        size_t GetSize() const override
        {
            return this->width * this->height * this->GetPixelSize();
        }

        void* GetData() const override
        {
            return (void*)this->data.get();
        }

        bool Inside(int x, int y) const
        {
            return x >= 0 && x < this->GetWidth() && y >= 0 && y < this->GetHeight();
        }

        size_t GetPixelSize() const
        {
            return GetPixelFormatBlockSize(this->format);
        }

        static bool ValidPixelFormat(PixelFormat format)
        {
            bool isColorFormat      = IsPixelFormatColor(format);
            bool isCompressedFormat = IsPixelFormatCompressed(format);

            return isColorFormat && !isCompressedFormat;
        }

        PixelSetFunction GetPixelSetFunction()
        {
            return this->pixelSetFunction;
        }

        PixelGetFunction GetPixelGetFunction()
        {
            return this->pixelGetFunction;
        }

      protected:
        ImageData(PixelFormat format, int width, int height) :
            format(format),
            width(width),
            height(height)
        {}

        PixelFormat format;
        int width;
        int height;

        PixelSetFunction pixelSetFunction;
        PixelGetFunction pixelGetFunction;

        love::mutex mutex;
        std::unique_ptr<uint8_t[]> data;
    };
} // namespace love
