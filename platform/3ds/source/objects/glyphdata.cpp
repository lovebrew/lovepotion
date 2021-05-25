#include "objects/glyphdata/glyphdata.h"

using namespace love;

common::GlyphData::GlyphData(const GlyphData& other) : glyph(other.glyph), metrics(other.metrics)
{}
