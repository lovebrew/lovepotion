#include <common/exception.hpp>

#include <modules/fontmodule_ext.hpp>

#include <objects/truetyperasterizer/truetyperasterizer.tcc>

#include <utilities/shaper/genericshaper.hpp>

#include <cmath>

using namespace love;

static float scaleMetric(uint8_t value, float scale)
{
    return value * scale;
}

template<>
TrueTypeRasterizer<Console::CTR>::TrueTypeRasterizer(FT_Library, Data* data, int size,
                                                     float dpiScale,
                                                     TrueTypeRasterizer<>::Hinting hinting)
{
    this->dpiScale = dpiScale;

    this->face = (CFNT_s*)linearAlloc(data->GetSize());
    std::memcpy((uint8_t*)this->face, data->GetData(), data->GetSize());
    fontFixPointers(this->face);

    auto* fontInfo  = fontGetInfo(this->face);
    auto* sheetInfo = fontInfo->tglp;

    this->scale = std::floor(size * dpiScale + 0.5f) / sheetInfo->cellHeight;

    if (size <= 0 || scale <= 0)
        throw Exception("Invalid TrueType font size: %d", size);

    this->data.Set(data);
    this->hinting = hinting;

    this->metrics.advance = scaleMetric(sheetInfo->maxCharWidth, scale);
    this->metrics.ascent  = scaleMetric(fontInfo->ascent, scale);
    this->metrics.descent = scaleMetric((fontInfo->height - fontInfo->ascent), scale);
    this->metrics.height  = scaleMetric(sheetInfo->cellHeight, scale);
}

template<>
TrueTypeRasterizer<Console::CTR>::~TrueTypeRasterizer()
{
    if (this->face)
        linearFree(this->face);
}

template<>
TextShaper* TrueTypeRasterizer<Console::CTR>::NewTextShaper()
{
    return new GenericShaper(this);
}

template<>
int TrueTypeRasterizer<Console::CTR>::GetLineHeight() const
{
    return 1;
}

template<>
bool TrueTypeRasterizer<Console::CTR>::HasGlyph(uint32_t glyph) const
{
    int index        = fontGlyphIndexFromCodePoint(this->face, glyph);
    const auto* info = fontGetInfo(this->face);

    return index != info->alterCharIndex && glyph != '\t';
}

template<>
int TrueTypeRasterizer<Console::CTR>::GetGlyphSpacing(uint32_t glyph) const
{
    fontGlyphPos_s out {};

    int index = fontGlyphIndexFromCodePoint(this->face, glyph);
    fontCalcGlyphPos(&out, this->face, index, GLYPH_POS_CALC_VTXCOORD, this->scale, this->scale);

    return out.xAdvance;
}

template<>
int TrueTypeRasterizer<Console::CTR>::GetGlyphIndex(uint32_t glyph) const
{
    const auto index      = fontGlyphIndexFromCodePoint(this->face, glyph);
    this->glyphMap[glyph] = index;

    return index;
}

template<>
GlyphData* TrueTypeRasterizer<Console::CTR>::GetGlyphDataForIndex(int index) const
{
    fontGlyphPos_s out {};
    fontCalcGlyphPos(&out, this->face, index, GLYPH_POS_CALC_VTXCOORD, this->scale, this->scale);

    GlyphData::GlyphMetrics metrics {};
    metrics.height   = this->metrics.height;
    metrics.width    = out.width;
    metrics.advance  = out.xAdvance;
    metrics.bearingX = out.xOffset;
    metrics.bearingY = this->metrics.ascent;

    GlyphData::GlyphSheetInfo sheetInfo {};
    sheetInfo.index = out.sheetIndex;

    sheetInfo.top    = out.texcoord.top;
    sheetInfo.left   = out.texcoord.left;
    sheetInfo.right  = out.texcoord.right;
    sheetInfo.bottom = out.texcoord.bottom;

    return new GlyphData(0, metrics, sheetInfo, PIXELFORMAT_RGBA8_UNORM);
}

template<>
int TrueTypeRasterizer<Console::CTR>::GetGlyphCount() const
{
    if (this->glyphCount != -1)
        return this->glyphCount;

    /* cache this data, as it's slow and stupid */
    FINF_s* info = fontGetInfo(this->face);
    int count    = 0;

    for (auto map = info->cmap; map; map = map->next)
        count += (map->codeEnd - map->codeBegin) + 1;

    return this->glyphCount = count;
}

template<>
float TrueTypeRasterizer<Console::CTR>::GetKerning(uint32_t, uint32_t) const
{
    return 0.0f;
}

template<>
Rasterizer::DataType TrueTypeRasterizer<Console::CTR>::GetDataType() const
{
    return Rasterizer::DataType::DATA_BCFNT;
}

template<>
bool TrueTypeRasterizer<Console::CTR>::Accepts(FT_Library, Data* data)
{
    return (!std::memcmp(data->GetData(), "CFNT", 4) || !std::memcmp(data->GetData(), "CFNU", 4));
}
