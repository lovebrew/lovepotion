#pragma once

#include "objects/glyphdata/glyphdatac.h"

namespace love
{
    class GlyphData : public common::GlyphData
    {
      public:
        GlyphData(uint32_t glyph, GlyphMetrics metrics);

        GlyphData(const GlyphData& other);

        GlyphData* Clone() const override;
    };
} // namespace love
