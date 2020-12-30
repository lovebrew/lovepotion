#pragma once

#include "freetype/rasterizer.h"
#include "freetype/truetyperasterizer.h"

#include "objects/filedata/filedata.h"
#include "objects/font/font.h"

#include <switch/types.h>

#include "common/module.h"

namespace love
{
    class FontModule : public Module
    {
        public:
            FontModule();

            virtual ~FontModule();

            Rasterizer * NewRasterizer(love::FileData * data);

            Rasterizer * NewTrueTypeRasterizer(size_t size, float dpiScale, TrueTypeRasterizer::Hinting hinting);

            Rasterizer * NewTrueTypeRasterizer(love::Data * data, int size, TrueTypeRasterizer::Hinting hinting);

            Rasterizer * NewTrueTypeRasterizer(love::Data * data, int size, float dpiScale, TrueTypeRasterizer::Hinting hinting);

            Rasterizer * NewTrueTypeRasterizer(int size, TrueTypeRasterizer::Hinting hinting);

            ModuleType GetModuleType() const { return M_FONT; }

            GlyphData * NewGlyphData(Rasterizer * rasterizer, const std::string & text);

            GlyphData * NewGlyphData(Rasterizer * rasterizer, uint32_t glyph);

            Data * GetSystemFont(Font::SystemFontType type);

            const char * GetName() const override { return "love.font"; }

        private:
            FT_Library library;
    };
}