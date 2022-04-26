#pragma once

#include "common/colors.h"
#include "common/data.h"
#include "common/pixelformat.h"

#include "objects/filedata/filedata.h"
#include "objects/imagedata/imagedatabase.h"

#include "objects/imagedata/types/formathandler.h"

#include "thread/types/mutex.h"

#include <vector>

namespace love
{
    class ImageData;

    namespace common
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

            typedef void (*PixelSetFunction)(const Colorf& c, Pixel* p);
            typedef void (*PixelGetFunction)(const Pixel* p, Colorf& c);

            static love::Type type;

            ImageData(Data* data);

            ImageData(int width, int height, PixelFormat format = PIXELFORMAT_RGBA8_UNORM);

            ImageData(int width, int height, PixelFormat format, void* data, bool own);

            ImageData(const ImageData& other);

            virtual ~ImageData();

            void Paste(ImageData* source, int dx, int dy, int sx, int sy, int sw, int sh);

            virtual void PasteData(ImageData* source, int dx, int dy, int sx, int sy, int sw,
                                   int sh, int dw, int dh) = 0;

            virtual void SetPixel(int x, int y, const Colorf& color) = 0;

            virtual void GetPixel(int x, int y, Colorf& color) const = 0;

            bool Inside(int x, int y) const;

            Colorf GetPixel(int x, int y) const;

            FileData* Encode(FormatHandler::EncodedFormat encodedFormat, const char* filename,
                             bool writefile) const;

            thread::Mutex* GetMutex() const;

            void* GetData() const override;

            size_t GetSize() const override;

            bool IsSRGB() const override;

            size_t GetPixelSize() const;

            static bool ValidatePixelFormat(PixelFormat format);

            static bool CanPaste(PixelFormat source, PixelFormat destination);

            static bool GetConstant(const char* in, FormatHandler::EncodedFormat& out);

            static bool GetConstant(FormatHandler::EncodedFormat in, const char*& out);

            static std::vector<const char*> GetConstants(FormatHandler::EncodedFormat);

            static PixelSetFunction GetPixelSetFunction(PixelFormat format);

            static PixelGetFunction GetPixelGetFunction(PixelFormat format);

          protected:
            union Row
            {
                uint8_t* u8;
                uint16_t* u16;
            };

            void Create(int width, int height, PixelFormat format, void* data = nullptr);

            void Decode(Data* data);

            uint8_t* data = nullptr;

            thread::MutexRef mutex;

            StrongReference<FormatHandler> decodeHandler;

            PixelSetFunction pixelSetFunction;
            PixelGetFunction pixelGetFunction;
        };
    } // namespace common
} // namespace love
