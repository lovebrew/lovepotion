#pragma once

#include <common/color.hpp>
#include <common/console.hpp>
#include <common/data.hpp>
#include <common/exception.hpp>
#include <common/math.hpp>
#include <common/pixelformat.hpp>

#include <objects/data/filedata/filedata.hpp>
#include <objects/imagedata/imagedatabase.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>
#include <utilities/formathandler/formathandler.hpp>

#include <utilities/threads/threads.hpp>

#if defined(__3DS__)
    #include <utilities/formathandler/types/t3xhandler.hpp>
#else
    #include <utilities/formathandler/types/astchandler.hpp>
    #include <utilities/formathandler/types/ddshandler.hpp>
    #include <utilities/formathandler/types/ktxhandler.hpp>
    #include <utilities/formathandler/types/pkmhandler.hpp>

    #include <utilities/formathandler/types/jpghandler.hpp>
    #include <utilities/formathandler/types/pnghandler.hpp>
#endif

#include <functional>
#include <memory>

namespace love
{
    template<Console::Platform T = Console::ALL>
    class ImageData : public ImageDataBase
    {
      public:
        union Pixel
        {
            uint8_t rgba8[4];
            uint16_t rgba16[4];
            float rgba32f[4];
            uint16_t packed16;
            uint32_t packed32;
        };

        union Row
        {
            uint8_t* u8;
            uint16_t* u16;
            // float16_t* f16;
            float_t* f32;
        };

        static inline Type type = Type("ImageData", &Object::type);

        typedef void (*PixelSetFunction)(const Color&, Pixel*);
        typedef void (*PixelGetFunction)(const Pixel*, Color&);

        static void pasteRGBA8toRGBA16(Row src, Row dst, int w)
        {
            for (int i = 0; i < w * 4; i++)
                dst.u16[i] = (uint16_t)src.u8[i] << 8u;
        }

        static void pasteRGBA16toRGBA8(Row src, Row dst, int w)
        {
            for (int i = 0; i < w * 4; i++)
                dst.u8[i] = src.u16[i] >> 8u;
        }

        static void setPixelRGBA8(const Color& color, Pixel* pixel)
        {
            if (Console::Is(Console::CTR))
            {
                pixel->packed32 = color.abgr();
                return;
            }

            pixel->rgba8[0] = static_cast<uint8_t>(std::clamp<float>(color.r, 0, 1) * 0xFF);
            pixel->rgba8[1] = static_cast<uint8_t>(std::clamp<float>(color.g, 0, 1) * 0xFF);
            pixel->rgba8[2] = static_cast<uint8_t>(std::clamp<float>(color.b, 0, 1) * 0xFF);
            pixel->rgba8[3] = static_cast<uint8_t>(std::clamp<float>(color.a, 0, 1) * 0xFF);
        }

        static void getPixelRGBA8(const Pixel* pixel, Color& color)
        {
            if (Console::Is(Console::CTR))
            {
                color = Color(pixel->packed32);
                return;
            }

            color.r = pixel->rgba8[0] / 0xFF;
            color.g = pixel->rgba8[1] / 0xFF;
            color.b = pixel->rgba8[2] / 0xFF;
            color.a = pixel->rgba8[3] / 0xFF;
        }

        static void setPixelRGBA16(const Color& color, Pixel* pixel)
        {
            pixel->rgba16[0] =
                static_cast<uint16_t>(std::clamp<float>(color.r, 0, 1) * 0xFFFF + 0.5f);
            pixel->rgba16[1] =
                static_cast<uint16_t>(std::clamp<float>(color.b, 0, 1) * 0xFFFF + 0.5f);
            pixel->rgba16[2] =
                static_cast<uint16_t>(std::clamp<float>(color.g, 0, 1) * 0xFFFF + 0.5f);
            pixel->rgba16[3] =
                static_cast<uint16_t>(std::clamp<float>(color.a, 0, 1) * 0xFFFF + 0.5f);
        }

        static void getPixelRGBA16(const Pixel* pixel, Color& color)
        {
            color.r = pixel->rgba16[0] / 0xFFFF;
            color.g = pixel->rgba16[1] / 0xFFFF;
            color.b = pixel->rgba16[2] / 0xFFFF;
            color.a = pixel->rgba16[3] / 0xFFFF;
        }

        ImageData(Data* data) : ImageDataBase(PIXELFORMAT_UNKNOWN, 0, 0)
        {
            this->Decode(data);
        }

        ImageData(int width, int height, PixelFormat format = PIXELFORMAT_RGBA8_UNORM) :
            ImageDataBase(format, width, height)
        {
            const char* formatName = love::GetPixelFormatName(format);
            if (!ValidPixelFormat(format))
                throw love::Exception("ImageData does not support the %s pixel format", formatName);

            this->Create(width, height, format);
            std::memset(this->data.get(), 0, this->GetSize());
        }

        ImageData(int width, int height, PixelFormat format, void* data, bool own) :
            ImageDataBase(format, width, height)
        {
            const char* formatName = love::GetPixelFormatName(format);
            if (!ValidPixelFormat(format))
                throw love::Exception("ImageData does not support the %s pixel format", formatName);

            if (own)
                this->data.reset((uint8_t*)data);
            else
                this->Create(width, height, format, data);
        }

        ImageData(const ImageData& other) : ImageDataBase(other.format, other.width, other.height)
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

        static bool Outside(int destX, int destY, int destWidth, int destHeight, const Rect& source)
        {
            return source.x >= source.w || source.x + source.w < 0 || source.y >= source.h ||
                   source.y + source.h < 0 || destX >= destWidth || destX + destWidth < 0 ||
                   destY >= destHeight || destY + destHeight < 0;
        }

        void AdjustPaste(const ImageData* source, int& x, int& y, const int width, const int height,
                         Rect& sourceRect)
        {
            if (Outside(x, y, width, height, sourceRect))
                return;

            // Normalize values to the inside of both images.
            if (x < 0)
            {
                sourceRect.w += x;
                sourceRect.x -= x;
                x = 0;
            }

            if (y < 0)
            {
                sourceRect.h += y;
                sourceRect.y -= y;
                y = 0;
            }

            if (sourceRect.x < 0)
            {
                sourceRect.w += sourceRect.x;
                x -= sourceRect.x;
                sourceRect.x = 0;
            }

            if (sourceRect.y < 0)
            {
                sourceRect.h += sourceRect.y;
                y -= sourceRect.y;
                sourceRect.y = 0;
            }

            sourceRect.w = std::min(sourceRect.w, width - x);
            sourceRect.h = std::min(sourceRect.h, height - y);

            sourceRect.w = std::min(sourceRect.w, source->GetWidth() - sourceRect.x);
            sourceRect.h = std::min(sourceRect.h, source->GetHeight() - sourceRect.y);
        }

        void Paste(ImageData* source, int x, int y, Rect& sourceRect)
        {
            PixelFormat destFormat = this->GetFormat();
            PixelFormat srcFormat  = source->GetFormat();

            const auto srcWidth  = source->GetWidth();
            const auto srcHeight = source->GetHeight();

            const auto destWidth  = this->GetWidth();
            const auto destHeight = this->GetHeight();

            size_t srcPixelSize = source->GetPixelSize();
            size_t dstPixelSize = this->GetPixelSize();

            this->AdjustPaste(source, x, y, destWidth, destHeight, sourceRect);

            std::unique_lock lock(source->mutex);
            std::unique_lock other(this->mutex);

            uint8_t* srcData = (uint8_t*)source->GetData();
            uint8_t* dstData = (uint8_t*)this->GetData();

            auto getFunction = source->pixelGetFunction;
            auto setFunction = this->pixelSetFunction;

            if (srcFormat == destFormat && (sourceRect.w == destWidth && destWidth == srcWidth &&
                                            sourceRect.h == destHeight && destHeight == srcHeight))
            {
                std::memcpy(dstData, srcData, source->GetSize());
            }
            else if (sourceRect.w > 0)
            {
                // Otherwise, copy each row individually.
                for (int i = 0; i < sourceRect.h; i++)
                {
                    Row rowsrc = { srcData +
                                   (sourceRect.x + (i + sourceRect.y) * srcWidth) * srcPixelSize };
                    Row rowdst = { dstData + (x + (i + y) * destWidth) * dstPixelSize };

                    if (srcFormat == destFormat)
                        std::memcpy(rowdst.u8, rowsrc.u8, srcPixelSize * sourceRect.w);
                    else
                    {
                        // Slow path: convert src -> Colorf -> dst.
                        Color color {};
                        for (int x = 0; x < sourceRect.w; x++)
                        {
                            auto srcPixel = (const Pixel*)(rowsrc.u8 + x * srcPixelSize);
                            auto dstPixel = (Pixel*)(rowdst.u8 + x * dstPixelSize);

                            getFunction(srcPixel, color);
                            setFunction(color, dstPixel);
                        }
                    }
                }
            }
        }

        void SetPixel(int x, int y, const Color& color)
        {
            if (!this->Inside(x, y))
                throw love::Exception("Attempt to set out-of-range pixel!");

            size_t size = this->GetPixelSize();
            auto* pixel = (Pixel*)(this->data.get() + (y * this->width + x) * size);

            if (this->pixelSetFunction == nullptr)
            {
                const char* formatName = love::GetPixelFormatName(this->format);
                throw love::Exception("ImageData:setPixel does not support the %s pixel format",
                                      formatName);
            }

            std::unique_lock lock(this->mutex);
            this->pixelSetFunction(color, pixel);
        }

        virtual void GetPixel(int x, int y, Color& color) const
        {
            if (!this->Inside(x, y))
                throw love::Exception("Attempt to set out-of-range pixel!");

            size_t size       = this->GetPixelSize();
            const auto* pixel = (const Pixel*)(this->data.get() + (y * this->width + x) * size);

            if (this->pixelGetFunction == nullptr)
            {
                const char* formatName = love::GetPixelFormatName(this->format);
                throw love::Exception("ImageData:getPixel does not support the %s pixel format",
                                      formatName);
            }

            std::unique_lock lock(this->mutex);
            this->pixelGetFunction(pixel, color);
        }

        virtual Color GetPixel(int x, int y) const
        {
            Color color {};
            this->GetPixel(x, y, color);

            return color;
        }

        FileData* Encode(FormatHandler::EncodedFormat format, const char* filename,
                         bool writeFile) const
        {
            return nullptr;
        }

        bool IsSRGB() const override
        {
            return false;
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

        PixelGetFunction GetPixelGetFunction(PixelFormat format)
        {
            switch (format)
            {
                case PIXELFORMAT_RGBA8_UNORM:
                    return getPixelRGBA8;
                case PIXELFORMAT_RGBA16_UNORM:
                    return getPixelRGBA16;
                default:
                    return nullptr;
            }
        }

        PixelSetFunction GetPixelSetFunction(PixelFormat format)
        {
            switch (format)
            {
                case PIXELFORMAT_RGBA8_UNORM:
                    return setPixelRGBA8;
                case PIXELFORMAT_RGBA16_UNORM:
                    return setPixelRGBA16;
                default:
                    return nullptr;
            }
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
        void Create(int width, int height, PixelFormat format, void* data = nullptr)
        {
            const auto size = GetPixelFormatSliceSize(format, width, height);

            try
            {
                this->data = std::make_unique<uint8_t[]>(size);
                std::memset(this->data.get(), 0, size);
            }
            catch (std::bad_alloc&)
            {
                throw love::Exception("Out of memory.");
            }

            if (data)
            {
                if (Console::Is(Console::CTR))
                {
                    if (width % 8 != 0 && height % 8 != 0)
                        throw love::Exception(
                            "Cannot create ImageData that is not a multiple of 8.");

                    if (format == PIXELFORMAT_RGB565_UNORM)
                        this->CopyBytesTiled<uint16_t>(data, width, height);
                    else
                        this->CopyBytesTiled<uint32_t>(data, width, height);
                }
                else
                    std::copy_n((uint8_t*)data, size, this->data.get());
            }

            this->decoder = nullptr;
            this->format  = format;

            this->pixelSetFunction = GetPixelSetFunction(format);
            this->pixelGetFunction = GetPixelGetFunction(format);
        }

        virtual void Decode(Data* data);

        std::unique_ptr<uint8_t[]> data;
        mutable love::mutex mutex;
        StrongReference<FormatHandler> decoder;

        PixelSetFunction pixelSetFunction;
        PixelGetFunction pixelGetFunction;
    };
} // namespace love
