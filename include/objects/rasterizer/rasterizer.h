#pragma once

#include "objects/glyphdata/glyphdata.h"
namespace love
{
    class Rasterizer : public Object
    {
      public:
        struct FontMetrics
        {
            int advance;
            int ascent;
            int descent;
            int height;
        };

        enum DataType
        {
            DATA_TRUETYPE,
            DATA_IMAGE,
            DATA_BCFNT
        };

        static love::Type type;

        virtual ~Rasterizer();

        virtual int GetHeight() const;

        virtual int GetAdvance() const;

        virtual int GetAscent() const;

        virtual int GetDescent() const;

        virtual int GetLineHeight() const = 0;

        virtual GlyphData* GetGlyphData(uint32_t glyph) const = 0;

        virtual GlyphData* GetGlyphData(const std::string& text) const;

        virtual int GetGlyphCount() const = 0;

        virtual bool HasGlyph(uint32_t glyph) const = 0;

        virtual bool HasGlyphs(const std::string& text) const;

        virtual float GetKerning(uint32_t left, uint32_t right) const;

        virtual DataType GetDataType() const = 0;

        float GetDPIScale() const;

      protected:
        FontMetrics metrics;
        float dpiScale = 1.0f;
    };
} // namespace love
