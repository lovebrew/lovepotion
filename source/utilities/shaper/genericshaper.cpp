#include <utilities/shaper/genericshaper.hpp>

#include <objects/rasterizer/rasterizer.hpp>

#include <optional>

using namespace love;

GenericShaper::GenericShaper(Rasterizer* rasterizer) : TextShaper(rasterizer)
{}

void GenericShaper::ComputeGlyphPositions(const ColoredCodepoints& codepoints, Range range,
                                          Vector2 offset, float spacing,
                                          std::vector<GlyphPosition>* positions,
                                          std::vector<IndexedColor>* colors, TextInfo* info)
{
    if (!range.isValid())
        range = Range(0, codepoints.cps.size());

    if (this->rasterizers[0]->GetDataType() == Rasterizer::DATA_TRUETYPE)
        offset.y += this->GetBaseline();

    Vector2 currentPosition = offset;

    int maxWidth           = 0;
    uint32_t previousGlyph = 0;

    if (positions)
        positions->reserve(range.getSize());

    int colorIndex = 0;
    int nColors    = (int)codepoints.colors.size();

    std::optional<Color> colorToAdd {};

    if (colors && range.getOffset() > 0 && !codepoints.colors.empty())
    {
        for (; colorIndex < nColors; colorIndex++)
        {
            if (codepoints.colors[colorIndex].index >= (int)range.getOffset())
                break;

            colorToAdd = codepoints.colors[colorIndex].color;
        }
    }

    for (int index = (int)range.getMin(); index <= (int)range.getMax(); index++)
    {
        uint32_t glyph = codepoints.cps[index];

        if (colors && colorIndex < nColors && codepoints.colors[colorIndex].index == index)
        {
            colorToAdd = codepoints.colors[colorIndex].color;
            colorIndex++;
        }

        if (glyph == '\n')
        {
            if (currentPosition.x > maxWidth)
                maxWidth = currentPosition.x;

            currentPosition.y += std::floor(this->GetHeight() + this->GetLineHeight() + 0.5f);
            currentPosition.x = offset.x;
            previousGlyph     = 0;

            continue;
        }

        if (glyph == '\r')
        {
            previousGlyph = glyph;
            continue;
        }

        if (colorToAdd.has_value() && colors && positions)
        {
            IndexedColor color = { colorToAdd.value(), (int)positions->size() };
            colors->push_back(color);
            colorToAdd.reset();
        }

        currentPosition.x += this->GetKerning(previousGlyph, glyph);

        GlyphIndex glyphIndex {};
        int advance = this->GetGlyphAdvance(glyph, &glyphIndex);

        if (positions)
        {
            Vector2 position { currentPosition.x, currentPosition.y };
            positions->push_back({ position, glyphIndex });
        }

        currentPosition.x += advance;

        if (glyph == ' ' && spacing != 0.0f)
            currentPosition.x += std::floor(currentPosition.x + spacing);

        previousGlyph = glyph;
    }

    if (currentPosition.x > maxWidth)
        maxWidth = currentPosition.x;

    if (info != nullptr)
    {
        info->width  = maxWidth - offset.x;
        info->height = currentPosition.y - offset.y;

        if (currentPosition.x > offset.x)
            info->height += std::floor(this->GetHeight() * this->GetLineHeight() + 0.5f);
    }
}

int GenericShaper::ComputeWordWrapIndex(const ColoredCodepoints& codepoints, Range range,
                                        float wrapLimit, float* width)
{
    if (!range.isValid())
        range = Range(0, codepoints.cps.size());

    uint32_t previousGlyph = 0;

    float currentWidth = 0.0f;
    float outWidth     = 0.0f;

    float widthBeforeLastSpace = 0.0f;
    int firstIndexAfterSpace   = -1;

    for (int index = (int)range.getMin(); index <= (int)range.getMax(); index++)
    {
        uint32_t glyph = codepoints.cps[index];

        if (glyph == '\r')
        {
            previousGlyph = glyph;
            continue;
        }

        int advance    = this->GetGlyphAdvance(glyph);
        float newWidth = currentWidth + this->GetKerning(previousGlyph, glyph) + advance;

        if (this->IsWhitespace(glyph))
        {
            if (!this->IsWhitespace(previousGlyph))
                widthBeforeLastSpace = currentWidth;
        }
        else
        {
            if (this->IsWhitespace(previousGlyph))
                firstIndexAfterSpace = index;

            if (newWidth > wrapLimit)
            {
                int wrapIndex = index > (int)range.first ? index : (int)range.first + 1;

                if (firstIndexAfterSpace != -1)
                {
                    wrapIndex = firstIndexAfterSpace;
                    outWidth  = widthBeforeLastSpace;
                }

                if (width)
                    *width = outWidth;

                return wrapIndex;
            }

            outWidth = newWidth;
        }

        currentWidth  = newWidth;
        previousGlyph = glyph;
    }

    if (width)
        *width = outWidth;

    return range.last + 1;
}