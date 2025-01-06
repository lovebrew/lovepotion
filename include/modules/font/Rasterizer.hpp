#pragma once

#include "common/Object.hpp"
#include "common/Optional.hpp"
#include "common/StrongRef.hpp"
#include "common/int.hpp"

#include "modules/font/GlyphData.hpp"

namespace love
{
    class TextShaper;

    struct FontMetrics
    {
        float advance;
        float ascent;
        float descent;
        float height;
    };

    class Rasterizer : public Object
    {
      public:
        enum Hinting
        {
            HINTING_NORMAL,
            HINTING_LIGHT,
            HINTING_MONO,
            HINTING_NONE,
            HINTING_MAX_ENUM
        };

        struct Settings
        {
            Hinting hinting = HINTING_NORMAL;
            OptionalFloat dpiScale;
            bool sdf = false;
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

        virtual int getHeight() const
        {
            return this->metrics.height;
        }

        virtual int getAdvance() const
        {
            return this->metrics.advance;
        }

        virtual int getAscent() const
        {
            return this->metrics.ascent;
        }

        virtual int getDescent() const
        {
            return this->metrics.descent;
        }

        virtual int getLineHeight() const = 0;

        virtual int getGlyphSpacing(uint32_t glyph) const = 0;

        virtual int getGlyphIndex(uint32_t glyph) const = 0;

        GlyphData* getGlyphData(uint32_t glyph) const;

        GlyphData* getGlyphData(const std::string& text) const;

        virtual GlyphData* getGlyphDataForIndex(int index) const = 0;

        virtual int getGlyphCount() const = 0;

        virtual bool hasGlyph(uint32_t glyph) const = 0;

        virtual bool hasGlyphs(const std::string& text) const;

        virtual float getKerning(uint32_t left, uint32_t right) const;

        virtual DataType getDataType() const = 0;

        virtual ptrdiff_t getHandle() const
        {
            return 0;
        }

        virtual TextShaper* newTextShaper() = 0;

        float getDPIScale() const
        {
            return dpiScale;
        }

        // clang-format off
        STRINGMAP_DECLARE(Hintings, Hinting,
            { "normal", HINTING_NORMAL },
            { "light",  HINTING_LIGHT  },
            { "mono",   HINTING_MONO   },
            { "none",   HINTING_NONE   }
        );
        // clang-format on

      protected:
        FontMetrics metrics;
        float dpiScale;

        StrongRef<Data> data;
        int size;
    };
} // namespace love
