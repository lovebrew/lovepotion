#pragma once

#include "objects/glyphdata/glyphdatac.h"

namespace love
{
    class GlyphData : public common::GlyphData
    {
        public:
            GlyphData* Clone() const override;
    };
} // namespace love
