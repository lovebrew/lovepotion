#pragma once

#include "common/Map.hpp"

#include "modules/filesystem/FileData.hpp"
#include "modules/font/Rasterizer.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

namespace love
{
    class TrueTypeRasterizer : public Rasterizer
    {
      public:
        TrueTypeRasterizer(FT_Library library, Data* data, int size, const Settings& settings,
                           float defaultDPIScale);

        virtual ~TrueTypeRasterizer();

        int getLineHeight() const override;

        int getGlyphSpacing(uint32_t glyph) const override;

        int getGlyphIndex(uint32_t glyph) const override;

        GlyphData* getGlyphDataForIndex(int index) const override;

        int getGlyphCount() const override;

        bool hasGlyph(uint32_t glyph) const override;

        float getKerning(uint32_t left, uint32_t right) const override;

        DataType getDataType() const override;

        TextShaper* newTextShaper() override;

        ptrdiff_t getHandle() const override
        {
            return (ptrdiff_t)this->face;
        }

        static bool accepts(FT_Library library, Data* data);

        // clang-format off
        STRINGMAP_DECLARE(Hintings, Hinting,
            { "normal", HINTING_NORMAL },
            { "light",  HINTING_LIGHT  },
            { "mono",   HINTING_MONO   },
            { "none",   HINTING_NONE   }
        );
        // clang-format on

      private:
        static FT_UInt hintingToLoadOption(Hinting hint);

        FT_Face face;
        Hinting hinting;
        bool sdf;
    };
} // namespace love
