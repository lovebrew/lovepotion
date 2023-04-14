#pragma once

#include <common/color.hpp>
#include <common/console.hpp>
#include <common/data.hpp>
#include <common/exception.hpp>
#include <common/math.hpp>
#include <common/pixelformat.hpp>

#include <objects/data/filedata/filedata.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>
#include <utilities/formathandler/formathandler.hpp>
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

        typedef void (*PixelSetFunction)(const Color&, Pixel*);
        typedef void (*PixelGetFunction)(const Pixel*, Color&);

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

        void CopyBytes(const void* buffer, const size_t size)
        {
            if (buffer != nullptr)
                std::memcpy(this->data.get(), buffer, size);
        }

        template<typename V>
        void CopyBytesTiled(const void* buffer, const int width, const int height)
        {
            if (width % 8 != 0 && height % 8 != 0)
                throw love::Exception("Cannot create ImageData that is not a multiple of 8.");

            const auto powTwoWidth = NextPo2(width);

            V* destination = (V*)this->data.get();
            V* source      = (V*)buffer;

            for (int j = 0; j < height; j += 8)
            {
                std::memcpy(destination, source, width * 8 * sizeof(V));

                source += width * 8;
                destination += powTwoWidth * 8;
            }
        }

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

        love::mutex& GetMutex()
        {
            return this->mutex;
        }

        // clang-format off
        static constexpr BidirectionalMap encodedFormats = {
            "png", FormatHandler::ENCODED_PNG,
            "exr", FormatHandler::ENCODED_EXR
        };
        // clang-format on

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
