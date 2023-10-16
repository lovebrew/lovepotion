#pragma once

#include <utilities/shaper/textshaper.hpp>

namespace love
{
    class GenericShaper : public TextShaper
    {
      public:
        GenericShaper(Rasterizer* rasterizer);

        virtual ~GenericShaper()
        {}

        void ComputeGlyphPositions(const ColoredCodepoints& codepoints, Range range, Vector2 offset,
                                   float extraspacing, std::vector<GlyphPosition>* positions,
                                   std::vector<IndexedColor>* colors, TextInfo* info) override;

        int ComputeWordWrapIndex(const ColoredCodepoints& codepoints, Range range, float wraplimit,
                                 float* width) override;
    };
} // namespace love