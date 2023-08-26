#include <modules/fontmodule_ext.hpp>

#include <objects/rasterizer_ext.hpp>

using namespace love;

Rasterizer<Console::CTR>::Rasterizer(Data* data, int size) : glyphCount(-1), data(data)
{
    this->face = FontModule<Console::CTR>::LoadFontFromFile(data->GetData(), data->GetSize());

    if (!this->face)
        throw love::Exception("BCFNT loading error (problem with font file?)");

    this->InitMetrics(size);
}

Rasterizer<Console::CTR>::Rasterizer(CFG_Region region, int size) : glyphCount(-1)
{
    this->face = FontModule<Console::CTR>::LoadSystemFont(region);

    this->InitMetrics(size);
}

Rasterizer<Console::CTR>::Rasterizer(CFNT_s* face) : glyphCount(-1)
{
    this->face = face;
}

Rasterizer<Console::CTR>::~Rasterizer()
{
    if (this->face)
        linearFree(this->face);
}

int Rasterizer<Console::CTR>::Scale(uint8_t value)
{
    return value * this->scale;
}

void Rasterizer<Console::CTR>::InitMetrics(int size)
{
    this->dpiScale = 1.0f;

    FINF_s* fontInfo  = fontGetInfo(this->face);
    TGLP_s* sheetInfo = fontInfo->tglp;

    this->scale = std::floor(size + this->dpiScale + 0.5f) / sheetInfo->cellHeight;

    this->metrics.advance = this->Scale(sheetInfo->maxCharWidth);
    this->metrics.ascent  = this->Scale(fontInfo->ascent);
    this->metrics.descent = this->Scale((fontInfo->height - fontInfo->ascent));
    this->metrics.height  = this->Scale(sheetInfo->cellHeight);

    this->dataType = DATA_BCFNT;
}

GlyphData* Rasterizer<Console::CTR>::GetGlyphData(const std::string_view& text) const
{
    uint32_t codepoint = 0;

    try
    {
        codepoint = utf8::peek_next(text.begin(), text.end());
    }
    catch (utf8::exception& e)
    {
        throw love::Exception("UTF-8 decoding error: %s", e.what());
    }

    return this->GetGlyphData(codepoint);
}

GlyphData* Rasterizer<Console::CTR>::GetGlyphData(uint32_t glyph) const
{
    GlyphData::GlyphMetrics metrics {};
    fontGlyphPos_s out;

    int index = fontGlyphIndexFromCodePoint(this->face, glyph);
    fontCalcGlyphPos(&out, this->face, index, GLYPH_POS_CALC_VTXCOORD, this->scale, this->scale);

    metrics.height   = this->metrics.height;
    metrics.width    = out.width;
    metrics.advance  = out.xAdvance;
    metrics.bearingX = out.xOffset;
    metrics.bearingY = this->metrics.ascent;

    GlyphSheetInfo sheetInfo {};
    sheetInfo.index = out.sheetIndex;

    sheetInfo.top    = out.texcoord.top;
    sheetInfo.left   = out.texcoord.left;
    sheetInfo.right  = out.texcoord.right;
    sheetInfo.bottom = out.texcoord.bottom;

    this->glyphSheetInfo[glyph] = sheetInfo;

    return new GlyphData(glyph, metrics, PIXELFORMAT_RGBA8_UNORM);
}

Rasterizer<Console::CTR>::GlyphSheetInfo& Rasterizer<Console::CTR>::GetSheetInfo(
    uint32_t glyph) const
{
    return this->glyphSheetInfo[glyph];
}

int Rasterizer<Console::CTR>::GetGlyphCount() const
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

bool Rasterizer<Console::CTR>::HasGlyphs(const std::string_view& text) const
{
    if (text.size() == 0)
        return false;

    try
    {
        Utf8Iterator start(text.begin(), text.begin(), text.end());
        Utf8Iterator end(text.end(), text.begin(), text.end());

        while (start != end)
        {
            uint32_t codepoint = *start++;

            if (!this->HasGlyph(codepoint))
                return false;
        }
    }
    catch (utf8::exception& e)
    {
        throw love::Exception("UTF-8 decoding error: %s", e.what());
    }

    return true;
}

bool Rasterizer<Console::CTR>::HasGlyph(uint32_t glyph) const
{
    int index        = fontGlyphIndexFromCodePoint(this->face, glyph);
    const auto* info = fontGetInfo(this->face);

    return index != info->alterCharIndex;
}

float Rasterizer<Console::CTR>::GetKerning(uint32_t, uint32_t) const
{
    return 0.0f;
}

bool Rasterizer<Console::CTR>::Accepts(Data* data)
{
    return (!std::memcmp(data->GetData(), "CFNT", 4) || !std::memcmp(data->GetData(), "CFNU", 4));
}
