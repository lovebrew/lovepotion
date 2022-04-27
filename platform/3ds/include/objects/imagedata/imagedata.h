#pragma once

#include "objects/imagedata/imagedatac.h"

namespace love
{
    class ImageData : public common::ImageData
    {
      public:
        ImageData(Data* data);

        ImageData(int width, int height, PixelFormat format = PIXELFORMAT_RGBA8_UNORM);

        ImageData(int width, int height, PixelFormat format, void* data, bool own);

        ImageData(const ImageData& other);

        virtual ~ImageData();

        ImageData* Clone() const override;

        virtual void PasteData(common::ImageData* source, int dx, int dy, int sx, int sy, int sw,
                               int sh, int dw, int dh) override;

        virtual void SetPixel(int x, int y, const Colorf& color) override;

        virtual void GetPixel(int x, int y, Colorf& color) const override;
    };
} // namespace love
