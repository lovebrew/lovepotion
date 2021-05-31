#include "objects/bcfntrasterizer/bcfntrasterizer.h"

using namespace love;

BCFNTRasterizer::BCFNTRasterizer(Data* data, int size) : data(data), glyphCount(-1)
{
    this->font = C2D_FontLoadFromMem(data->GetData(), data->GetSize());

    if (!this->font)
        throw love::Exception(
            "BCFNT loading error: C2D_FontLoadFromMem failed (problem with font file?)");

    this->InitMetrics(size);
}

BCFNTRasterizer::BCFNTRasterizer(common::Font::SystemFontType type, int size) : glyphCount(-1)
{
    this->font = C2D_FontLoadSystem(static_cast<CFG_Region>(type));

    this->InitMetrics(size);
}

void BCFNTRasterizer::InitMetrics(int size)
{
    /* Set global metrics */
    FINF_s* s                = C2D_FontGetInfo(this->font);
    TGLP_s* textureSheetInfo = s->tglp;

    this->scale = size / 30.0f;

    this->metrics.advance = (int)textureSheetInfo->maxCharWidth * this->scale;
    this->metrics.ascent  = (int)s->ascent * this->scale;
    this->metrics.descent = (int)(s->height - s->ascent) * this->scale;
    this->metrics.height  = (int)s->height * this->scale;
}

BCFNTRasterizer::~BCFNTRasterizer()
{
    C2D_FontFree(this->font);
}

GlyphData* BCFNTRasterizer::GetGlyphData(uint32_t glyph) const
{
    GlyphData::GlyphMetrics gMetrics {};

    int glyphIndex = C2D_FontGlyphIndexFromCodePoint(this->font, glyph);

    fontGlyphPos_s glyphPosition;
    C2D_FontCalcGlyphPos(this->font, &glyphPosition, glyphIndex, 0, this->scale, this->scale);

    gMetrics.height   = this->metrics.height;
    gMetrics.width    = glyphPosition.width;
    gMetrics.advance  = glyphPosition.xAdvance;
    gMetrics.bearingX = glyphPosition.xOffset;
    gMetrics.bearingY = this->metrics.ascent;

    return new GlyphData(glyph, gMetrics);
}

int BCFNTRasterizer::GetGlyphCount() const
{
    if (this->glyphCount != -1)
        return this->glyphCount;

    /* cache this data, as it's slow and stupid */

    FINF_s* fontInfo = C2D_FontGetInfo(this->font);

    for (auto map = fontInfo->cmap; map; map = map->next)
        this->glyphCount += (map->codeEnd - map->codeBegin) + 1;

    return this->glyphCount;
}

bool BCFNTRasterizer::HasGlyph(uint32_t glyph) const
{
    int glyphIndex   = C2D_FontGlyphIndexFromCodePoint(this->font, glyph);
    FINF_s* fontInfo = C2D_FontGetInfo(this->font);

    return glyphIndex != fontInfo->alterCharIndex;
}

float BCFNTRasterizer::GetKerning(uint32_t /* leftGlyph */, uint32_t /* rightGlyph */) const
{
    return 0.0f;
}

bool BCFNTRasterizer::Accepts(Data* data)
{
    return (!memcmp(data->GetData(), "CFNT", 4) || !memcmp(data->GetData(), "CFNU", 4));
}
