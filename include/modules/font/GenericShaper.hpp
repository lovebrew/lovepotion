#pragma once

#include "modules/font/TextShaper.hpp"

namespace love
{
    class GenericShaper : public TextShaper
    {
      public:
        GenericShaper(Rasterizer* rasterizer);

        virtual ~GenericShaper()
        {}

        void computeGlyphPositions(const ColoredCodepoints& codepoints, Range range, Vector2 offset,
                                   float extraSpacing, std::vector<GlyphPosition>* positions,
                                   std::vector<IndexedColor>* colors, TextInfo* info) override;

        int computeWordWrapIndex(const ColoredCodepoints& codepoints, Range range, float wraplimit,
                                 float* width) override;
    };
} // namespace love
