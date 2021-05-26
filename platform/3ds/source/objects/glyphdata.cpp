#include "objects/glyphdata/glyphdata.h"

using namespace love;

GlyphData::GlyphData(uint32_t glyph, GlyphMetrics metrics) : common::GlyphData(glyph, metrics)
{}

GlyphData::GlyphData(const GlyphData& other) : common::GlyphData(other)
{}

GlyphData* GlyphData::Clone() const
{
    return new GlyphData(*this);
}
