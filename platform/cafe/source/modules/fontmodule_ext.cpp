#include <modules/fontmodule_ext.hpp>

#include <modules/window_ext.hpp>

using namespace love;

FontModule<Console::CAFE>::FontModule()
{
    this->defaultFontData.Set(new SystemFont(), Acquire::NORETAIN);

    if (auto error = FT_Init_FreeType(&this->library); error != FT_Err_Ok)
        throw love::Exception("Failed to initialize FreeType: (0x%x)", error);
}

FontModule<Console::CAFE>::~FontModule()
{
    FT_Done_FreeType(this->library);
}

Rasterizer<Console::CAFE>* FontModule<Console::CAFE>::NewRasterizer(FileData* data) const
{
    if (Rasterizer<Console::CAFE>::Accepts(this->library, data))
        return this->NewTrueTypeRasterizer(data, 12, Rasterizer<Console::CAFE>::HINTING_NORMAL);

    throw love::Exception("Invalid font file: %s", data->GetFilename().c_str());
}

Rasterizer<Console::CAFE>* FontModule<Console::CAFE>::NewTrueTypeRasterizer(
    Data* data, int size, Rasterizer<Console::CAFE>::Hinting hinting) const
{
    float dpiScale = 1.0f;

    auto* window = Module::GetInstance<Window<Console::HAC>>(Module::M_WINDOW);

    if (window != nullptr)
        dpiScale = window->GetDPIScale();

    return this->NewTrueTypeRasterizer(data, size, dpiScale, hinting);
}

Rasterizer<Console::CAFE>* FontModule<Console::CAFE>::NewTrueTypeRasterizer(
    int size, Rasterizer<Console::CAFE>::Hinting hinting) const
{
    return this->NewTrueTypeRasterizer(this->defaultFontData.Get(), size, hinting);
}

Rasterizer<Console::CAFE>* FontModule<Console::CAFE>::NewTrueTypeRasterizer(
    int size, float dpiscale, Rasterizer<Console::CAFE>::Hinting hinting) const
{
    return this->NewTrueTypeRasterizer(this->defaultFontData.Get(), size, dpiscale, hinting);
}

Rasterizer<Console::CAFE>* FontModule<Console::CAFE>::NewTrueTypeRasterizer(
    love::Data* data, int size, float dpiscale, Rasterizer<Console::CAFE>::Hinting hinting) const
{
    return new Rasterizer<Console::CAFE>(this->library, data, size, dpiscale, hinting);
}

GlyphData* FontModule<Console::CAFE>::NewGlyphData(Rasterizer<Console::CAFE>* rasterizer,
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

GlyphData* FontModule<Console::CAFE>::NewGlyphData(Rasterizer<Console::CAFE>* rasterizer,
                                                   uint32_t glyph) const
{
    return rasterizer->GetGlyphData(glyph);
}
