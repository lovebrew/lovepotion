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

        Rasterizer* NewRasterizer(FileData* data) override;

        Rasterizer* NewTrueTypeRasterizer(
            int size, TrueTypeRasterizer::Hinting hinting,
            common::Font::SystemFontType type = common::Font::SystemFontType::TYPE_STANDARD);

        Rasterizer* NewTrueTypeRasterizer(
            int size, float dpiScale, TrueTypeRasterizer::Hinting hinting,
            common::Font::SystemFontType type = common::Font::SystemFontType::TYPE_STANDARD);

        Rasterizer* NewTrueTypeRasterizer(Data* data, int size,
                                          TrueTypeRasterizer::Hinting hinting);

        Rasterizer* NewTrueTypeRasterizer(Data* data, int size, float dpiScale,
                                          TrueTypeRasterizer::Hinting hinting);
    };
} // namespace love
