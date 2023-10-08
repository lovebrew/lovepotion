#pragma once

#include <common/object.hpp>

#include <objects/glyphdata/glyphdata.hpp>

namespace love
{
    class TextShaper;

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

        static Type type;

        virtual ~Rasterizer()
        {}

        int GetAdvance() const
        {
            return this->metrics.advance;
        }

        int GetAscent() const
        {
            return this->metrics.ascent;
        }

        int GetDescent() const
        {
            return this->metrics.descent;
        }

        int GetHeight() const
        {
            return this->metrics.height;
        }

        virtual int GetLineHeight() const = 0;

        virtual int GetGlyphSpacing(uint32_t glyph) const = 0;

        virtual int GetGlyphIndex(uint32_t glyph) const = 0;

        GlyphData* GetGlyphData(uint32_t glyph) const;

        GlyphData* GetGlyphData(const std::string& text) const;

        virtual GlyphData* GetGlyphDataForIndex(int index) const = 0;

        virtual int GetGlyphCount() const = 0;

        virtual bool HasGlyph(uint32_t glyph) const = 0;

        virtual bool HasGlyphs(const std::string& text) const;

        virtual float GetKerning(uint32_t left, uint32_t right) const;

        float GetDPIScale() const
        {
            return this->dpiScale;
        }

        virtual DataType GetDataType() const = 0;

        virtual void* GetHandle() const
        {
            return nullptr;
        }

        virtual TextShaper* NewTextShaper() = 0;

      protected:
        FontMetrics metrics;
        float dpiScale;
    };
} // namespace love
