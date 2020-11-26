#pragma once

#include "freetype/rasterizer.h"
#include "truetyperasterizer.h"

#include "objects/filedata/filedata.h"
#include "objects/font/font.h"

namespace love
{
    class FontModule : public Module
    {
        public:
            FontModule();

            ~FontModule();

            Rasterizer * NewRasterizer(love::FileData * data);

            Rasterizer * NewTrueTypeRasterizer(size_t size, float dpiScale, TrueTypeRasterizer::Hinting hinting);

            Rasterizer * NewTrueTypeRasterizer(void * data, size_t dataSize, int size, love::TrueTypeRasterizer::Hinting hinting);

            Rasterizer * NewTrueTypeRasterizer(love::Data * data, int size, TrueTypeRasterizer::Hinting hinting);

            Rasterizer * NewTrueTypeRasterizer(int size, TrueTypeRasterizer::Hinting hinting);

            ModuleType GetModuleType() const { return M_FONT; }

            GlyphData * NewGlyphData(Rasterizer * rasterizer, const std::string & text);

            Data * GetSystemFont(Font::SystemFontType type);

            const char * GetName() const override { return "love.font"; }

        private:
            FT_Library library;
    };
}