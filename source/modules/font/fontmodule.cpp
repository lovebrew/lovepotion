#include <modules/window_ext.hpp>

#include <modules/font/fontmodule.tcc>

#include <objects/truetyperasterizer/truetyperasterizer.tcc>

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