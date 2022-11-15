#pragma once

#include <objects/rasterizer/rasterizer.tcc>

#include <common/strongreference.hpp>

#include <objects/glyphdata/glyphdata.hpp>

#include <citro2d.h>

namespace love
{
    template<>
    class Rasterizer<Console::CTR> : public Rasterizer<Console::ALL>
    {
      public:
        Rasterizer(Data* data, int size);

        Rasterizer(CFG_Region region, int size);

        virtual ~Rasterizer();

        int GetLineHeight() const
        {
            return 1;
        }

        float GetScale() const
        {
            return this->scale;
        }

        C2D_Font GetFont() const
        {
            return this->font;
        }

        GlyphData* GetGlyphData(const std::string& text) const;

        GlyphData* GetGlyphData(uint32_t glyph) const;

        int GetGlyphCount() const;

        bool HasGlyph(uint32_t glyph) const;

        bool HasGlyphs(const std::string_view& text) const;

        float GetKerning(uint32_t left, uint32_t right) const;

        static bool Accepts(Data* data);

        int Scale(uint8_t in);

      private:
        C2D_Font font;
        float scale;
        mutable int glyphCount;

        StrongReference<Data> data;
        void InitMetrics(int size);
    };
} // namespace love
