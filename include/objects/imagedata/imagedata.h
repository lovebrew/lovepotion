#pragma once

#include "common/colors.h"
#include "common/data.h"
#include "common/pixelformat.h"
#include "common/stringmap.h"

#include "objects/filedata/filedata.h"
#include "objects/imagedata/imagedatabase.h"

#include "objects/imagedata/types/formathandler.h"

#include "thread/types/mutex.h"

namespace love
{
    class ImageData : public ImageDataBase
    {
      public:
        union Pixel
        {
            uint8_t rgba8[4];
            uint16_t rgba16[4];
            // float16_t rgba16f[4];
            float rgba32f[4];
            uint16_t packed16;
            uint32_t packed32;
        };

        static love::Type type;

        ImageData(Data* data);

        ImageData(int width, int height, PixelFormat format = PIXELFORMAT_RGBA8);

        ImageData(int width, int height, PixelFormat format, void* data, bool own);

        ImageData(const ImageData& other);

        virtual ~ImageData();

        void Paste(ImageData* source, int dx, int dy, int sx, int sy, int sw, int sh);

        bool Inside(int x, int y) const;

        void SetPixel(int x, int y, const Colorf& color);

        void GetPixel(int x, int y, Colorf& color) const;

        Colorf GetPixel(int x, int y) const;

        FileData* Encode(/* TODO */);

        thread::Mutex* GetMutex() const;

        ImageData* Clone() const override;

        void* GetData() const override;

        size_t GetSize() const override;

        bool IsSRGB() const override;

        size_t GetPixelSize() const;

        static bool ValidatePixelFormat(PixelFormat format);

        static bool CanPaste(PixelFormat source, PixelFormat destination);

      private:
        void Create(int width, int height, PixelFormat format, void* data = nullptr);

        void Decode(Data* data);

        uint8_t* data = nullptr;

        thread::MutexRef mutex;

        StrongReference<FormatHandler> decodeHandler;

        static StringMap<FormatHandler::EncodedFormat, FormatHandler::ENCODED_MAX_ENUM>::Entry
            encodedFormatEntries[];
        static StringMap<FormatHandler::EncodedFormat, FormatHandler::ENCODED_MAX_ENUM>
            encodedFormats;
    };
} // namespace love
