#pragma once

#include <common/data.hpp>
#include <common/pixelformat.hpp>

namespace love
{
    class ImageDataBase : public Data
    {
      public:
        virtual ~ImageDataBase()
        {}

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