#pragma once

#include <objects/rasterizer/rasterizer.tcc>

#include <common/strongreference.hpp>

#include <citro2d.h>

namespace love
{
    template<>
    class Rasterizer<Console::CTR> : public Rasterizer<Console::ALL>
    {
      public:
        struct GlyphSheetInfo
        {
            int index;
            float left;
            float top;
            float right;
            float bottom;
        };

        Rasterizer(Data* data, int size);

        Rasterizer(CFG_Region region, int size);

        Rasterizer(CFNT_s* face);

        virtual ~Rasterizer();

        int GetLineHeight() const
        {
            return 1;
        }

        float GetScale() const
        {
            return this->scale;
        }

        CFNT_s* GetFont() const
        {
            return this->face;
        }

        GlyphData* GetGlyphData(const std::string_view& text) const;

        GlyphData* GetGlyphData(uint32_t glyph) const;

        GlyphSheetInfo& GetSheetInfo(uint32_t glyph) const;

        int GetGlyphCount() const;

        bool HasGlyphs(const std::string_view& text) const;

        bool HasGlyph(uint32_t glyph) const;

        float GetKerning(uint32_t left, uint32_t right) const;

        static bool Accepts(Data* data);

        int Scale(uint8_t in);

      private:
        CFNT_s* face;
        float scale;
        mutable int glyphCount;

        StrongReference<Data> data;
        void InitMetrics(int size);

        mutable std::unordered_map<uint32_t, GlyphSheetInfo> glyphSheetInfo;
    };
} // namespace love
