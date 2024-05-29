#include "modules/font/Font.tcc"

#include "utf8.h"

namespace love
{
    Rasterizer* FontModuleBase::newTrueTypeRasterizer(int size, const Rasterizer::Settings& settings) const
    {
        return this->newTrueTypeRasterizer(this->defaultFontData.get(), size, settings);
    }

    GlyphData* FontModuleBase::newGlyphData(Rasterizer* rasterizer, const std::string& text) const
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

        return rasterizer->getGlyphData(codepoint);
    }

    GlyphData* FontModuleBase::newGlyphData(Rasterizer* rasterizer, uint32_t codepoint) const
    {
        return rasterizer->getGlyphData(codepoint);
    }
} // namespace love
