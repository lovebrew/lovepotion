#pragma once

#include "common/Color.hpp"
#include "common/Data.hpp"
#include "common/Map.hpp"
#include "common/float.hpp"
#include "common/int.hpp"
#include "common/pixelformat.hpp"

#include "modules/filesystem/FileData.hpp"

#include <pthread.h>

#include "modules/image/FormatHandler.hpp"
#include "modules/image/ImageDataBase.hpp"

namespace love
{
    class ImageData : public ImageDataBase
    {
      public:
        union Pixel
        {
            uint8_t rgba8[4];
            uint16_t rgba16[4];
            float16_t rgba16f[4];
            float rgba32f[4];
            uint16_t packed16;
            uint32_t packed32;
        };

        using PixelSetFunction = void (*)(const Color& color, Pixel* pixel);
        using PixelGetFunction = void (*)(const Pixel* pixel, Color& color);

        static Type type;

        ImageData(Data* data);

        ImageData(int width, int height, PixelFormat format);

        ImageData(int width, int height, PixelFormat format, void* data, bool own);

        ImageData(const ImageData& other);

        virtual ~ImageData();

        void paste(ImageData* source, int dx, int dy, int sx, int sy, int sw, int sh);

        bool inside(int x, int y) const;

        void setPixel(int x, int y, const Color& color);

        void getPixel(int x, int y, Color& color) const;

        Color getPixel(int x, int y) const;

        FileData* encode(FormatHandler::EncodedFormat format, const char* filename, bool writeFile) const;

        ImageData* clone() const override;

        void* getData() const override;

        size_t getSize() const override;

        size_t getPixelSize() const;

        PixelSetFunction getPixelSetFunction() const
        {
            return this->pixelSetFunction;
        }

        PixelGetFunction getPixelGetFunction() const
        {
            return this->pixelGetFunction;
        }

        static bool validPixelFormat(PixelFormat format);

        static PixelSetFunction getPixelSetFunction(PixelFormat format);

        static PixelGetFunction getPixelGetFunction(PixelFormat format);

        // clang-format off
        STRINGMAP_DECLARE(EncodedFormats, FormatHandler::EncodedFormat,
            { "png", FormatHandler::ENCODED_PNG },
            { "tga", FormatHandler::ENCODED_TGA },
            { "exr", FormatHandler::ENCODED_EXR }
        );
        // clang-format on

      private:
        void create(int width, int height, PixelFormat format, void* data = nullptr);

        void decode(Data* data);

        uint8_t* data = nullptr;
        StrongRef<FormatHandler> decodeHandler;

        PixelSetFunction pixelSetFunction;
        PixelGetFunction pixelGetFunction;
    };
} // namespace love
