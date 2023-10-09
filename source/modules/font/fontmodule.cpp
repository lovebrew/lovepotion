#include <modules/window_ext.hpp>

#include <modules/font/fontmodule.tcc>

#include <objects/truetyperasterizer/truetyperasterizer.tcc>

#include <objects/imagerasterizer/imagerasterizer.hpp>

#include <objects/bmfontrasterizer/bmfontrasterizer.hpp>

using namespace love;

template<>
FontModule<Console::ALL>::FontModule()
{
#if !defined(__3DS__)
    if (auto error = FT_Init_FreeType(&this->library); error != FT_Err_Ok)
        throw love::Exception("Failed to initialize FreeType: (%s)", FT_Error_String(error));
#endif
}

template<>
FontModule<Console::ALL>::~FontModule()
{
#if !defined(__3DS__)
    FT_Done_FreeType(this->library);
#endif
}

template<>
Rasterizer* FontModule<Console::ALL>::NewBMFontRasterizer(
    FileData* data, const std::vector<ImageData<Console::Which>*>& images, float dpiScale) const
{
    return new BMFontRasterizer(data, images, dpiScale);
}

template<>
Rasterizer* FontModule<Console::ALL>::NewImageRasterizer(ImageData<Console::Which>* data,
                                                         const std::string& text, int extraSpacing,
                                                         float dpiScale) const
{

    std::vector<uint32_t> glyphs {};
    glyphs.reserve(text.size());

    try
    {
        utf8::iterator<std::string::const_iterator> it(text.begin(), text.begin(), text.end());
        utf8::iterator<std::string::const_iterator> end(text.end(), text.begin(), text.end());

        while (it != end)
            glyphs.push_back(*it++);
    }
    catch (utf8::exception& e)
    {
        throw love::Exception("UTF-8 decoding error: %s", e.what());
    }

    return new ImageRasterizer(data, &glyphs[0], (int)glyphs.size(), extraSpacing, dpiScale);
}

template<>
Rasterizer* FontModule<Console::ALL>::NewImageRasterizer(ImageData<Console::Which>* data,
                                                         uint32_t* glyphs, int glyphCount,
                                                         int extraSpacing, float dpiScale) const
{
    return new ImageRasterizer(data, glyphs, glyphCount, extraSpacing, dpiScale);
}

template<>
Rasterizer* FontModule<Console::ALL>::NewTrueTypeRasterizer(
    int size, TrueTypeRasterizer<>::Hinting hinting) const
{
    return this->NewTrueTypeRasterizer(this->defaultFontData.Get(), size, hinting);
}

template<>
Rasterizer* FontModule<Console::ALL>::NewTrueTypeRasterizer(
    int size, float dpiScale, TrueTypeRasterizer<>::Hinting hinting) const
{
    return this->NewTrueTypeRasterizer(this->defaultFontData.Get(), size, dpiScale, hinting);
}

template<>
GlyphData* FontModule<Console::ALL>::NewGlyphData(Rasterizer* rasterizer,
                                                  const std::string& text) const
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

template<>
GlyphData* FontModule<Console::ALL>::NewGlyphData(Rasterizer* rasterizer, uint32_t glyph) const
{
    return rasterizer->GetGlyphData(glyph);
}