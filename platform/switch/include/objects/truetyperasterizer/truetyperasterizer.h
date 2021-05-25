#pragma once

#include "common/stringmap.h"
#include "objects/rasterizer/rasterizer.h"

#include "common/strongref.h"

// FreeType2
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

namespace love
{
    class TrueTypeRasterizer : public Rasterizer
    {
      public:
        /*
        ** Types of hinting
        ** for TrueType font glyphs.
        */
        enum Hinting
        {
            HINTING_NORMAL,
            HINTING_LIGHT,
            HINTING_MONO,
            HINTING_NONE,
            HINTING_MAX_ENUM
        };

        TrueTypeRasterizer(FT_Library library, love::Data* data, int size, Hinting hinting);

        virtual ~TrueTypeRasterizer();

        // Implement Rasterizer
        int GetLineHeight() const override;

        GlyphData* GetGlyphData(uint32_t glyph) const override;

        int GetGlyphCount() const override;

        bool HasGlyph(uint32_t glyph) const override;

        float GetKerning(uint32_t leftglyph, uint32_t rightglyph) const override;

        DataType GetDataType() const override;

        static bool Accepts(FT_Library library, love::Data* data);

        static bool GetConstant(const char* in, Hinting& out);
        static bool GetConstant(Hinting in, const char*& out);
        static std::vector<const char*> GetConstants(Hinting);

      private:
        FT_Face face;

        StrongReference<love::Data> data;

        Hinting hinting;

        const static StringMap<Hinting, HINTING_MAX_ENUM> hintings;

        static FT_UInt HintingToLoadOption(Hinting hinting);
    };
} // namespace love
