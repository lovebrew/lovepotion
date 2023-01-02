#include <modules/fontmodule_ext.hpp>

using namespace love;

FontModule<Console::CTR>::FontModule()
{
    this->defaultFontData.Set(new SystemFont(), Acquire::NORETAIN);
}

Rasterizer<Console::CTR>* FontModule<Console::CTR>::NewRasterizer(FileData* data) const
{
    if (Rasterizer<Console::CTR>::Accepts(data))
        return new Rasterizer<Console::CTR>(data, 12);

    throw love::Exception("Invalid font file: %s", data->GetFilename());
}

Rasterizer<Console::CTR>* FontModule<Console::CTR>::NewBCFNTRasterizer(int size)
{
    return this->NewBCFNTRasterizer(this->defaultFontData.Get(), size);
}

Rasterizer<Console::CTR>* FontModule<Console::CTR>::NewBCFNTRasterizer(Data* data, int size) const
{
    return new Rasterizer<Console::CTR>(data, size);
}

Rasterizer<Console::CTR>* FontModule<Console::CTR>::NewBCFNTRasterizer(int size,
                                                                       CFG_Region region) const
{
    return new Rasterizer<Console::CTR>(region, size);
}

GlyphData* FontModule<Console::CTR>::NewGlyphData(Rasterizer<Console::CTR>* rasterizer,
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

GlyphData* FontModule<Console::CTR>::NewGlyphData(Rasterizer<Console::CTR>* rasterizer,
                                                  uint32_t glyph) const
{
    return rasterizer->GetGlyphData(glyph);
}
