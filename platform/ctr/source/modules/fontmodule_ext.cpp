#include <modules/fontmodule_ext.hpp>

#include <objects/rasterizer_ext.hpp>

using namespace love;

static CFNT_s* loadFromArchive(uint64_t title, const char* path)
{
    std::unique_ptr<uint8_t[]> fontData;
    uint32_t size = 0;

    Result result = 0;
    CFNT_s* font  = nullptr;

    result = romfsMountFromTitle(title, MEDIATYPE_NAND, "font");

    if (R_FAILED(result))
        return nullptr;

    std::FILE* file = std::fopen(path, "rb");

    if (!file)
    {
        std::fclose(file);
        return nullptr;
    }

    std::fseek(file, 0, SEEK_END);
    size = (uint32_t)std::ftell(file);
    std::rewind(file);

    try
    {
        fontData = std::make_unique<uint8_t[]>(size);
    }
    catch (std::bad_alloc&)
    {
        throw love::Exception("Not enough memory.");
    }

    std::fread(fontData.get(), 1, size, file);
    std::fclose(file);

    romfsUnmount("font");

    uint32_t fontSize = *(uint32_t*)fontData.get() >> 0x08;
    font              = (CFNT_s*)linearAlloc(fontSize);

    if (font && !decompress_LZ11(font, fontSize, nullptr, fontData.get() + 4, size - 4))
    {
        linearFree(font);
        throw love::Exception("Failed to decompress '%s'", path);
    }

    fontFixPointers(font);

    return font;
}

static size_t getFontIndex(CFG_Region region)
{
    switch (region)
    {
        default:
        case CFG_REGION_JPN ... CFG_REGION_AUS:
            return 0;
        case CFG_REGION_CHN:
            return 1;
        case CFG_REGION_KOR:
            return 2;
        case CFG_REGION_TWN:
            return 3;
    }
}

CFNT_s* FontModule<Console::CTR>::LoadSystemFont(CFG_Region region)
{
    size_t index         = getFontIndex(region);
    uint8_t systemRegion = 0;

    Result result = CFGU_SecureInfoGetRegion(&systemRegion);

    if (R_FAILED(result) || index == getFontIndex((CFG_Region)systemRegion))
        return fontGetSystemFont();

    return loadFromArchive(FontModule::FONT_ARCHIVE | (index << 8), FontModule::fontPaths[index]);
}

CFNT_s* FontModule<Console::CTR>::LoadFontFromFile(const void* data, size_t size)
{
    CFNT_s* font = (CFNT_s*)linearAlloc(size);

    if (font != nullptr)
        std::memcpy((uint8_t*)font, data, size);
    else
        throw love::Exception("Failed to allocate font.");

    fontFixPointers(font);

    return font;
}

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
    return this->NewBCFNTRasterizer(size, CFG_REGION_USA);
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
