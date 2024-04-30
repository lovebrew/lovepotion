#pragma once

#include "common/Module.hpp"

#include "modules/filesystem/physfs/Filesystem.hpp"
#include "modules/image/CompressedImageData.hpp"
#include "modules/image/ImageData.hpp"

#include <list>

namespace love
{
    class Image : public Module
    {
      public:
        Image();

        virtual ~Image();

        ImageData* newImageData(Data* data) const;

        ImageData* newImageData(int width, int height, PixelFormat format = PIXELFORMAT_RGBA8_UNORM) const;

        ImageData* newImageData(int width, int height, PixelFormat format, void* data,
                                bool own = false) const;

        CompressedImageData* newCompressedData(Data* data) const;

        bool isCompressed(Data* data) const;

        const std::list<FormatHandler*>& getFormatHandlers() const;

      private:
        ImageData* newPastedImageData(ImageData* source, int sx, int sy, int width, int height) const;

        std::list<FormatHandler*> formatHandlers;
    };
} // namespace love
