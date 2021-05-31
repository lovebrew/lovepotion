#pragma once

#include "common/data.h"
#include "common/exception.h"

#include <string.h>

namespace love::common
{
    class GlyphData : public Data
    {
      public:
        static love::Type type;

        struct GlyphMetrics
        {
            int height;
            int width;
            int advance;
            int bearingX;
            int bearingY;
        };

        GlyphData(uint32_t glyph, GlyphMetrics metrics);

        GlyphData(const GlyphData& glyphData);

        virtual ~GlyphData();

        virtual GlyphData* Clone() const = 0;

        virtual size_t GetPixelSize() const
        {
            return 0;
        };

        virtual size_t GetSize() const
        {
            return 0;
        };

        virtual void* GetData() const override
        {
            return nullptr;
        }

        virtual void* GetData(int x, int y) const
        {
            return nullptr;
        }

        int GetWidth() const;

        int GetHeight() const;

        uint32_t GetGlyph() const;

        std::string GetGlyphString() const;

        int GetAdvance() const;

        int GetBearingX() const;

        int GetBearingY() const;

        int GetMinX() const;

        int GetMinY() const;

        int GetMaxX() const;

        int GetMaxY() const;

      protected:
        uint32_t glyph;
        GlyphMetrics metrics;
    };
} // namespace love::common
