#include <objects/rasterizer_ext.hpp>

using namespace love;

Rasterizer<Console::CTR>::Rasterizer(Data* data, int size) : glyphCount(-1), data(data)
{
    this->font = C2D_FontLoadFromMem(data->GetData(), data->GetSize());

    if (!this->font)
        throw love::Exception("BCFNT loading erorr (problem with font file?)");

    this->InitMetrics(size);
}

Rasterizer<Console::CTR>::Rasterizer(CFG_Region region, int size) : glyphCount(-1)
{
    this->font = C2D_FontLoadSystem(region);

    this->InitMetrics(size);
}

Rasterizer<Console::CTR>::~Rasterizer()
{
    C2D_FontFree(this->font);
}

int Rasterizer<Console::CTR>::Scale(uint8_t in)
{
    return in * this->scale;
}

void Rasterizer<Console::CTR>::InitMetrics(int size)
{
    FINF_s* fontInfo  = C2D_FontGetInfo(this->font);
    TGLP_s* sheetInfo = fontInfo->tglp;

    this->scale = size / 30.0f;

    this->metrics.advance = this->Scale(sheetInfo->maxCharWidth);
    this->metrics.ascent  = this->Scale(fontInfo->ascent);
    this->metrics.descent = this->Scale((fontInfo->height - fontInfo->ascent));
    this->metrics.height  = this->Scale(fontInfo->height);

    this->dataType = DATA_BCFNT;
}

GlyphData* Rasterizer<Console::CTR>::GetGlyphData(const std::string& text) const
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
    GlyphData::GlyphMetrics gMetrics {};

    int glyphIndex = C2D_FontGlyphIndexFromCodePoint(this->font, glyph);

    fontGlyphPos_s glyphPosition;
    C2D_FontCalcGlyphPos(this->font, &glyphPosition, glyphIndex, 0, this->scale, this->scale);

    gMetrics.height   = this->metrics.height;
    gMetrics.width    = glyphPosition.width;
    gMetrics.advance  = glyphPosition.xAdvance;
    gMetrics.bearingX = glyphPosition.xOffset;
    gMetrics.bearingY = this->metrics.ascent;

    return new GlyphData(glyph, gMetrics, PIXELFORMAT_RGBA8_UNORM);
}

int Rasterizer<Console::CTR>::GetGlyphCount() const
{
    if (this->glyphCount != -1)
        return this->glyphCount;

    /* cache this data, as it's slow and stupid */
    FINF_s* info = C2D_FontGetInfo(this->font);
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
    int index    = C2D_FontGlyphIndexFromCodePoint(this->font, glyph);
    FINF_s* info = C2D_FontGetInfo(this->font);

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
