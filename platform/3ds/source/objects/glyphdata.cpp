#include "objects/glyphdata/glyphdata.h"

using namespace love;

GlyphData* GlyphData::Clone() const
{
    return new GlyphData(*this);
}
