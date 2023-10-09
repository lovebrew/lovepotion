#include <modules/window_ext.hpp>

#include <modules/fontmodule_ext.hpp>

using namespace love;

SystemFont::SystemFont(CFG_Region region)
{
    this->font = FontModule<Console::CTR>::LoadSystemFont(region, this->size);
}

static CFNT_s* loadFromArchive(uint64_t title, const char* path, size_t& outSize)
{
    std::unique_ptr<uint8_t[]> fontData;
    long size = 0;

    Result result = 0;
    CFNT_s* font  = nullptr;

    result = romfsMountFromTitle(title, MEDIATYPE_NAND, "font");

    if (R_FAILED(result))
    {
        throw love::Exception("Failed to mount 'font:/' from NAND: %x", result);
        return nullptr;
    }

    auto* file = std::fopen(path, "rb");

    if (!file)
    {
        std::fclose(file);
        romfsUnmount("font");

        throw love::Exception("Failed to open '%s'", path);
        return nullptr;
    }

    std::fseek(file, 0, SEEK_END);
    size = std::ftell(file);
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

    font = (CFNT_s*)linearAlloc(fontSize);

    if (font && !decompress_LZ11(font, fontSize, nullptr, fontData.get() + 4, size - 4))
    {
        linearFree(font);
        throw love::Exception("Failed to decompress '%s'", path);
    }

    outSize = fontSize;

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

CFNT_s* FontModule<Console::CTR>::LoadSystemFont(CFG_Region region, size_t& size)
{
    size_t index = getFontIndex(region);

    const auto archive = FontModule::FONT_ARCHIVE | (index << 8);
    return loadFromArchive(archive, FontModule::fontPaths[index], size);
}

FontModule<Console::CTR>::FontModule()
{
    size_t size = 0;
    void* data  = FontModule<Console::CTR>::LoadSystemFont(CFG_REGION_USA, size);

    this->defaultFontData.Set(new ByteData(data, size, true), Acquire::NORETAIN);
}

Rasterizer* FontModule<Console::CTR>::NewRasterizer(FileData* data) const
{
    if (TrueTypeRasterizer<Console::CTR>::Accepts(this->library, data))
        return this->NewTrueTypeRasterizer(data, 12, TrueTypeRasterizer<>::HINTING_NORMAL);

    throw love::Exception("Invalid font file: %s", data->GetFilename().c_str());
    return nullptr;
}

Rasterizer* FontModule<Console::CTR>::NewTrueTypeRasterizer(
    Data* data, int size, TrueTypeRasterizer<>::Hinting hinting) const
{
    float dpiScale = 1.0f;
    auto window    = Module::GetInstance<Window<Console::Which>>(Module::M_WINDOW);

    if (window != nullptr)
        dpiScale = window->GetDPIScale();

    return this->NewTrueTypeRasterizer(data, size, dpiScale, hinting);
}

Rasterizer* FontModule<Console::CTR>::NewTrueTypeRasterizer(
    Data* data, int size, float dpiScale, TrueTypeRasterizer<>::Hinting hinting) const
{
    return new TrueTypeRasterizer<Console::CTR>(this->library, data, size, dpiScale, hinting);
}