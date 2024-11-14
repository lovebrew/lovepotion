#pragma once

#include "common/Data.hpp"
#include "common/pixelformat.hpp"

namespace love
{
    class ImageDataBase : public Data
    {
      public:
        virtual ~ImageDataBase()
        {}

        PixelFormat getFormat() const
        {
            return this->format;
        }

        int getWidth() const
        {
            return this->width;
        }

        int getHeight() const
        {
            return this->height;
        }

        void setLinear(bool linear)
        {
            this->linear = linear;
        }

        bool isLinear() const
        {
            return this->linear;
        }

      protected:
        ImageDataBase(PixelFormat format, int width, int height);

        PixelFormat format;
        int width;
        int height;

        bool linear;
    };
} // namespace love
