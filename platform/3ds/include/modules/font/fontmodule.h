#pragma once

#include "modules/font/fontmodulec.h"
#include "objects/bcfntrasterizer/bcfntrasterizer.h"

namespace love
{
    class FontModule : public common::FontModule
    {
      public:
        virtual ~FontModule() {};

        Rasterizer* NewRasterizer(FileData* data) override;

        Rasterizer* NewBCFNTRasterizer(Data* data, int size);

        Rasterizer* NewBCFNTRasterizer(int size, common::Font::SystemFontType type =
                                                     common::Font::SystemFontType::TYPE_STANDARD);
    };
} // namespace love
