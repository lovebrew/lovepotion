#pragma once

#include <common/module.hpp>

#include <objects/compressedimagedata/compressedimagedata.hpp>
#include <objects/imagedata_ext.hpp>

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

        ImageData<Console::Which>* NewImageData(Data* data) const;

        ImageData<Console::Which>* NewImageData(int width, int height,
                                                PixelFormat format = PIXELFORMAT_RGBA8_UNORM) const;

        ImageData<Console::Which>* NewImageData(int width, int height, PixelFormat format,
                                                void* data, bool own = false) const;

        CompressedImageData* NewCompressedImageData(Data* data) const;

        bool IsCompressed(Data* data) const;

        const std::list<love::FormatHandler*>& GetFormatHandlers() const;

      private:
        std::list<love::FormatHandler*> formatHandlers;
    };
} // namespace love
