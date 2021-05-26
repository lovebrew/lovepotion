#pragma once

#include "modules/font/fontmodulec.h"
#include "objects/truetyperasterizer/truetyperasterizer.h"

namespace love
{
    class FontModule : public common::FontModule
    {
      public:
        FontModule();

        virtual ~FontModule();

        Data* GetSystemFont(Font::SystemFontType type);

        Rasterizer* NewRasterizer(FileData* data) override;

        Rasterizer* NewTrueTypeRasterizer(int size, TrueTypeRasterizer::Hinting hinting);

        Rasterizer* NewTrueTypeRasterizer(int size, float dpiScale,
                                          TrueTypeRasterizer::Hinting hinting);

        Rasterizer* NewTrueTypeRasterizer(Data* data, int size,
                                          TrueTypeRasterizer::Hinting hinting);

        Rasterizer* NewTrueTypeRasterizer(Data* data, int size, float dpiScale,
                                          TrueTypeRasterizer::Hinting hinting);

      private:
        FT_Library library;
    };
} // namespace love
