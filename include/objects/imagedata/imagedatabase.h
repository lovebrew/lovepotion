#pragma once

#include "common/data.h"
#include "common/pixelformat.h"

namespace love
{
    class ImageDataBase : public Data
    {
      public:
        ~ImageDataBase() {};

        PixelFormat GetFormat() const;

        int GetWidth() const;

        int GetHeight() const;

        virtual bool IsSRGB() const = 0;

      protected:
        ImageDataBase(PixelFormat format, int width, int height);

        PixelFormat format;
        int width;
        int height;
    };
} // namespace love
