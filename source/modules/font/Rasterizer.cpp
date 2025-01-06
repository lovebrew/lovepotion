#include "modules/font/Rasterizer.hpp"

#include "utf8.h"

namespace love
{
    Type Rasterizer::type("Rasterizer", &Object::type);

    GlyphData* Rasterizer::getGlyphData(uint32_t glyph) const
    {
        return this->getGlyphDataForIndex(this->getGlyphIndex(glyph));
    }

    GlyphData* Rasterizer::getGlyphData(const std::string& text) const
    {
        uint32_t codepoint = 0;

        try
        {
            codepoint = utf8::peek_next(text.begin(), text.end());
        }
        catch (utf8::exception& e)
        {
            throw love::Exception("UTF-8 decoding error: {:s}", e.what());
        }

        return this->getGlyphData(codepoint);
    }

    bool Rasterizer::hasGlyphs(const std::string& text) const
    {
        if (text.size() == 0)
            return false;

        try
        {
            utf8::iterator<std::string::const_iterator> it(text.begin(), text.begin(), text.end());
            utf8::iterator<std::string::const_iterator> end(text.end(), text.begin(), text.end());

            while (it != end)
            {
                uint32_t codepoint = *it++;

                if (!this->hasGlyph(codepoint))
                    return false;
            }
        }
        catch (utf8::exception& e)
        {
            throw love::Exception("UTF-8 decoding error: {:s}", e.what());
        }

        return true;
    }

    float Rasterizer::getKerning(uint32_t, uint32_t) const
    {
        return 0.0f;
    }
} // namespace love
