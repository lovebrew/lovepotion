#pragma once

#include "objects/glyphdata/glyphdatac.h"

namespace love
{
    class GlyphData : public common::GlyphData
    {
      public:
        GlyphData(uint32_t glyph, GlyphMetrics metrics);

        GlyphData(const GlyphData& c);

        virtual ~GlyphData();

        GlyphData* Clone() const override;

        size_t GetPixelSize() const;

        void* GetData() const;

        size_t GetSize() const;

        void* GetData(int x, int y) const;

      private:
        uint8_t* data;
    };
} // namespace love
