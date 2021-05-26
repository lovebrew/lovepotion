#include "objects/glyphdata/glyphdatac.h"

#include "utf8/utf8.h"

using namespace love::common;

love::Type GlyphData::type("GlyphData", &Data::type);

GlyphData::GlyphData(uint32_t glyph, GlyphMetrics metrics) : glyph(glyph), metrics(metrics)
{}

GlyphData::GlyphData(const GlyphData& other) : glyph(other.glyph), metrics(other.metrics)
{}

GlyphData::~GlyphData()
{}

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
