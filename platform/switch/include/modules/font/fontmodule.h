#pragma once

#include "modules/font/fontmodulec.h"
#include "objects/truetyperasterizer/truetyperasterizer.h"

namespace love
{
    class DefaultFontData : public love::Data
    {
      public:
        DefaultFontData(Font::SystemFontType type = Font::SystemFontType::TYPE_STANDARD) :
            type(type)
        {
            plGetSharedFontByType(&this->fontData, (PlSharedFontType)type);
        }

        Data* Clone() const override
        {
            return new DefaultFontData(this->type);
        }

        void* GetData() const override
        {
            return this->fontData.address;
        }

        size_t GetSize() const override
        {
            return this->fontData.size;
        }

      private:
        PlFontData fontData;
        Font::SystemFontType type;
    };

    class FontModule : public common::FontModule
    {
      public:
        FontModule();

        virtual ~FontModule();

        Data* GetSystemFont(Font::SystemFontType type);

        Rasterizer* NewRasterizer(FileData* data) override;

        Rasterizer* NewTrueTypeRasterizer(Font::SystemFontType fontType, int size,
                                          TrueTypeRasterizer::Hinting hinting);

        Rasterizer* NewTrueTypeRasterizer(Font::SystemFontType fontType, int size, float dpiScale,
                                          TrueTypeRasterizer::Hinting hinting);

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
