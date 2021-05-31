#include "modules/font/fontmodulec.h"

using namespace love::common;

FontModule::FontModule()
{}

FontModule::~FontModule()
{}

love::GlyphData* FontModule::NewGlyphData(Rasterizer* rasterizer, const std::string& text)
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

    return rasterizer->GetGlyphData(codepoint);
}

love::GlyphData* FontModule::NewGlyphData(Rasterizer* rasterizer, uint32_t glyph)
{
    return rasterizer->GetGlyphData(glyph);
}
