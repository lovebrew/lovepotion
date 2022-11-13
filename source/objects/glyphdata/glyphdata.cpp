#include <objects/glyphdata/glyphdata.hpp>

using namespace love;

GlyphData::GlyphData(uint32_t glyph, GlyphMetrics metrics, PixelFormat format) :
    glyph(glyph),
    metrics(metrics),
    format(format)
{
    if (format != PIXELFORMAT_LA8_UNORM && format != PIXELFORMAT_RGBA8_UNORM)
        throw love::Exception("Invalid GlyphData pixel format!");

    if (this->metrics.width > 0 && this->metrics.height > 0)
        this->data = std::make_unique<uint8_t>(this->GetSize());
}

GlyphData::GlyphData(const GlyphData& other) :
    glyph(other.glyph),
    metrics(other.metrics),
    data(nullptr),
    format(other.format)
{
    if (metrics.width > 0 && metrics.height > 0)
    {
        data = std::make_unique<uint8_t>(this->GetSize());
        std::copy_n(other.data.get(), other.GetSize(), this->data.get());
    }
}

GlyphData* GlyphData::Clone() const
{
    return new GlyphData(*this);
}

void* GlyphData::GetData() const
{
    return this->data.get();
}

PixelFormat GlyphData::GetFormat() const
{
    return this->format;
}

size_t GlyphData::GetPixelSize() const
{
    return GetPixelFormatBlockSize(this->format);
}

size_t GlyphData::GetSize() const
{
    return size_t(this->GetWidth() * this->GetHeight() * this->GetPixelSize());
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
    char u[5]        = { 0, 0, 0, 0, 0 };
    ptrdiff_t length = 0;

    try
    {
        char* end = utf8::append(glyph, u);
        length    = end - u;
    }
    catch (utf8::exception& e)
    {
        throw love::Exception("UTF-8 decoding error: %s", e.what());
    }

    // Just in case...
    if (length < 0)
        return "";

    return std::string(u, length);
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
