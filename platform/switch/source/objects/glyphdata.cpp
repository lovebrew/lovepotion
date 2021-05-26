#include "objects/glyphdata/glyphdata.h"

#include "utf8/utf8.h"

using namespace love;

GlyphData::GlyphData(uint32_t glyph, GlyphMetrics metrics) :
    common::GlyphData(glyph, metrics),
    data(nullptr)
{
    size_t pixelSize = this->GetPixelSize();
    if (this->metrics.width > 0 && this->metrics.height > 0)
        this->data = new uint8_t[(metrics.width * metrics.height) * pixelSize];
}

GlyphData::GlyphData(const GlyphData& other) :
    common::GlyphData(other.glyph, other.metrics),
    data(nullptr)
{
    size_t pixelSize = this->GetPixelSize();
    if (this->metrics.width > 0 && this->metrics.height > 0)
    {
        data = new uint8_t[(metrics.width * metrics.height) * pixelSize];
        memcpy(data, other.data, other.GetSize());
    }
}

GlyphData* GlyphData::Clone() const
{
    return new GlyphData(*this);
}

size_t GlyphData::GetPixelSize() const
{
    return 4;
}

GlyphData::~GlyphData()
{
    delete[] this->data;
}

void* GlyphData::GetData() const
{
    return this->data;
}

void* GlyphData::GetData(int x, int y) const
{
    size_t offset = (y * this->GetWidth() + x) * this->GetPixelSize();
    return this->data + offset;
}

size_t GlyphData::GetSize() const
{
    return size_t(this->GetWidth() * this->GetHeight()) * this->GetPixelSize();
}
