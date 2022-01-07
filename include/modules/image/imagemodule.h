#pragma once

#include "common/module.h"
#include "objects/file/file.h"

#include "modules/data/wrap_datamodule.h"
#include "modules/filesystem/wrap_filesystem.h"

#include "objects/compressedimagedata/compressedimagedata.h"
#include "objects/imagedata/imagedata.h"

#include "objects/imagedata/types/formathandler.h"
#include "objects/imagedata/wrap_imagedata.h"

#include <list>

namespace love
{
    class ImageModule : public Module
    {
      public:
        ImageModule();

        virtual ~ImageModule();

        ModuleType GetModuleType() const override
        {
            return M_IMAGE;
        }

        const char* GetName() const override
        {
            return "love.image";
        }

        ImageData* NewImageData(Data* data);

#if defined(__SWITCH__)
        ImageData* NewImageData(int width, int height, PixelFormat format = PIXELFORMAT_RGBA8);
#elif defined(__3DS__)
        ImageData* NewImageData(int width, int height,
                                PixelFormat format = PIXELFORMAT_TEX3DS_RGBA8);
#endif
        ImageData* NewImageData(int width, int height, PixelFormat format, void* data,
                                bool own = false);

        CompressedImageData* NewCompressedData(Data* data);

        bool IsCompressed(Data* data);

        const std::list<FormatHandler*>& GetFormatHandlers() const;

        static bool GetConstant(PixelFormat in, const char*& out);

        static bool GetConstant(const char* in, PixelFormat& out);

      private:
        std::list<FormatHandler*> formatHandlers;

        const static StringMap<PixelFormat, PIXELFORMAT_MAX_ENUM> pixelFormats;
    };
} // namespace love
