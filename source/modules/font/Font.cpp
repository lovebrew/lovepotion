#include "modules/font/Font.tcc"

#include "common/Console.hpp"
#include "common/Exception.hpp"
#include "modules/filesystem/FileData.hpp"
#include "modules/font/BMFontRasterizer.hpp"
#include "modules/font/ImageRasterizer.hpp"
#include "utf8/checked.h"

namespace love
{
    Rasterizer* FontModuleBase::newTrueTypeRasterizer(int size, const Rasterizer::Settings& settings) const
    {
        return this->newTrueTypeRasterizer(this->defaultFontData.get(), size, settings);
    }

    Rasterizer* FontModuleBase::newBMFontRasterizer(FileData* definition,
                                                    const std::vector<ImageData*>& images, float dpiScale)
    {
        if constexpr (Console::is(Console::CTR))
        {
            throw love::Exception("BMFontRasterizer is not supported on this system.");
            return nullptr;
        }

        return new BMFontRasterizer(definition, images, dpiScale);
    }

    Rasterizer* FontModuleBase::newImageRasterizer(ImageData* data, const std::string& glyphs, int spacing,
                                                   float dpiScale)
    {
        if constexpr (Console::is(Console::CTR))
        {
            throw love::Exception("ImageRasterizer is not supported on this system.");
            return nullptr;
        }

        std::vector<uint32_t> chars;
        chars.reserve(glyphs.size());

        try
        {
            utf8::iterator<std::string::const_iterator> i(glyphs.begin(), glyphs.end(), glyphs.end());
            utf8::iterator<std::string::const_iterator> end(glyphs.end(), glyphs.begin(), glyphs.end());

            while (i != end)
                chars.push_back(*i++);
        }
        catch (utf8::exception& e)
        {
            throw love::Exception("UTF-8 decoding error: {:s}", e.what());
        }

        return new ImageRasterizer(data, chars.data(), (int)chars.size(), spacing, dpiScale);
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
