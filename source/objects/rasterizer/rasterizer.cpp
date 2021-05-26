#include "objects/rasterizer/rasterizer.h"

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

GlyphData* Rasterizer::GetGlyphData(const std::string& text) const
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

bool Rasterizer::HasGlyphs(const std::string& text) const
{
    if (text.size() == 0)
        return false;

    try
    {
        utf8::iterator<std::string::const_iterator> i(text.begin(), text.begin(), text.end());
        utf8::iterator<std::string::const_iterator> end(text.end(), text.begin(), text.end());

        while (i != end)
        {
            uint32_t codepoint = *i++;

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

float Rasterizer::GetKerning(uint32_t /*leftglyph*/, uint32_t /*rightglyph*/) const
{
    return 0.0f;
}

float Rasterizer::GetDPIScale() const
{
    return this->dpiScale;
}
