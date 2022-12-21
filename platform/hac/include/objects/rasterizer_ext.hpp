#pragma once

#include <objects/rasterizer/rasterizer.tcc>

#include <common/strongreference.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

namespace love
{
    template<>
    class Rasterizer<Console::HAC> : public Rasterizer<Console::ALL>
    {
      public:
        enum Hinting
        {
            HINTING_NORMAL,
            HINTING_LIGHT,
            HINTING_MONO,
            HINTING_NONE
        };

        Rasterizer(FT_Library library, Data* data, int size, float dpiSacale, Hinting hinting);

        ~Rasterizer();

        int GetLineHeight() const;

        GlyphData* GetGlyphData(const std::string_view& text) const;

        GlyphData* GetGlyphData(uint32_t glyph) const;

        int GetGlyphCount() const;

        bool HasGlyphs(const std::string_view& text) const;

        bool HasGlyph(uint32_t glyph) const;

        float GetKerning(uint32_t left, uint32_t right) const;

        static bool Accepts(FT_Library library, Data* data);

        // clang-format off
        static constexpr BidirectionalMap hintings = {
            "normal", HINTING_NORMAL,
            "light",  HINTING_LIGHT,
            "mono",   HINTING_MONO,
            "none",   HINTING_NONE
        };
        // clang-format on

      private:
        // clang-format off
        static constexpr BidirectionalMap loadOptions = {
            HINTING_NORMAL, FT_LOAD_TARGET_NORMAL,
            HINTING_LIGHT,  FT_LOAD_TARGET_LIGHT,
            HINTING_MONO,   FT_LOAD_TARGET_MONO,
            HINTING_NONE,   FT_LOAD_NO_HINTING
        };
        // clang-format on

        FT_Face face;
        StrongReference<Data> data;
        Hinting hinting;
    };
} // namespace love
