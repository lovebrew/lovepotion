#include "common/runtime.h"
#include "freetype/rasterizer.h"

#include "utf8/utf8.h"

using namespace love;

love::Type Rasterizer::type("Rasterizer", &Object::type);

Rasterizer::~Rasterizer()
{}

int Rasterizer::GetHeight() const
{
    return this->metrics.height;
}

int Rasterizer::GetAdvance() const
{
    return this->metrics.advance;
}

int Rasterizer::GetAscent() const
{
    return this->metrics.ascent;
}

int Rasterizer::GetDescent() const
{
    return this->metrics.descent;
}

GlyphData * Rasterizer::GetGlyphData(const std::string & text) const
{
    uint32_t codepoint = 0;

    try
    {
        codepoint = utf8::peek_next(text.begin(), text.end());
    }
    catch (utf8::exception &e)
    {
        throw love::Exception("UTF-8 decoding error: %s", e.what());
    }

    return GetGlyphData(codepoint);
}

bool Rasterizer::HasGlyphs(const std::string & text) const
{
    if (text.size() == 0)
        return false;

    auto start = text.begin();
    const auto end = text.end();

    while (start != end)
    {
        uint32_t codepoint = 0;

        auto bytes = decode_utf8(&codepoint, (uint8_t *)&*start);
        if (bytes < 0)
        {
            bytes = 1;
            codepoint = 0xFFFD;
        }

        if (!this->HasGlyph(codepoint))
            return false;

        start += bytes;
    }

    return true;
}

float Rasterizer::GetKerning(uint32_t /*leftglyph*/, uint32_t /*rightglyph*/) const
{
    return 0.0f;
}

float Rasterizer::GetDPIScale() const
{
    return this->dpiScale;
}