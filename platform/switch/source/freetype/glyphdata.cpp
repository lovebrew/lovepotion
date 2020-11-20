#include "common/runtime.h"
#include "freetype/glyphdata.h"

using namespace love;

love::Type GlyphData::type("GlyphData", &Data::type);

GlyphData::GlyphData(uint32_t glyph, GlyphMetrics metrics) : glyph(glyph),
                                                             metrics(metrics),
                                                             data(nullptr)
{
    if (this->metrics.width > 0 && this->metrics.height > 0)
        this->data = new uint8_t[metrics.width * metrics.height * this->GetPixelSize()];
}

GlyphData::GlyphData(const GlyphData & glyphData) : glyph(glyphData.glyph),
                                                    metrics(glyphData.metrics),
                                                    data(nullptr)
{
    if (this->metrics.width > 0 && this->metrics.height > 0)
    {
        data = new uint8_t[metrics.width * metrics.height * this->GetPixelSize()];
        memcpy(data, glyphData.data, glyphData.GetSize());
    }
}

/* always LA8 pixel format -- so 2 */
size_t GlyphData::GetPixelSize() const
{
    return 4;
}

GlyphData::~GlyphData()
{
    delete [] this->data;
}

GlyphData * GlyphData::Clone() const
{
    return new GlyphData(*this);
}

void * GlyphData::GetData() const
{
    return this->data;
}

void * GlyphData::GetData(int x, int y) const
{
    size_t offset = (y * this->GetWidth() + x) * this->GetPixelSize();
    return this->data + offset;
}

size_t GlyphData::GetSize() const
{
    return size_t(this->GetWidth() * this->GetHeight()) * this->GetPixelSize();
}

int GlyphData::GetHeight() const
{
    return this->metrics.height;
}

int GlyphData::GetWidth() const
{
    return this->metrics.width;
}

uint32_t GlyphData::GetGlyph() const
{
    return this->glyph;
}

std::string GlyphData::GetGlyphString() const
{
    uint8_t out[5] = {0, 0, 0, 0, 0};
    auto bytes = encode_utf8(out, this->glyph);

    if (bytes < 0)
        bytes = encode_utf8(out, 0xFFFD);

    return std::string((char *)out, bytes);
}

int GlyphData::GetAdvance() const
{
    return this->metrics.advance;
}

int GlyphData::GetBearingX() const
{
    return this->metrics.bearingX;
}

int GlyphData::GetBearingY() const
{
    return this->metrics.bearingY;
}

int GlyphData::GetMinX() const
{
    return this->GetBearingX();
}

int GlyphData::GetMinY() const
{
    return this->GetHeight() - this->GetBearingY();
}

int GlyphData::GetMaxX() const
{
    return this->GetBearingX() + this->GetWidth();
}

int GlyphData::GetMaxY() const
{
    return this->GetBearingY();
}