#include "modules/font/Font.hpp"
#include "common/Result.hpp"

#include "modules/font/BCFNTRasterizer.hpp"

#include <string.h>

namespace love
{
    // #region System Font
    SystemFont::SystemFont(uint32_t dataType) : data(nullptr), size(0), dataType(dataType)
    {
        this->data = FontModule::loadSystemFont(CFG_REGION_USA, this->size);
    }

    SystemFont::~SystemFont()
    {
        linearFree(this->data);
    }
    // #endregion

    SystemFont* FontModule::loadSystemFontByType(SystemFontType type)
    {
        return new SystemFont(type);
    }

    static CFNT_s* loadFromArchive(uint64_t title, const char* path, size_t& outSize)
    {
        std::unique_ptr<uint8_t[]> data;
        long size = 0;

        CFNT_s* font = nullptr;

        if (auto result = Result(romfsMountFromTitle(title, MEDIATYPE_NAND, "font")); result.failed())
        {
            throw love::Exception("Failed to mount 'font:/' from NAND: {:X}", result.get());
            return nullptr;
        }

        std::FILE* file = std::fopen(path, "rb");

        if (!file)
        {
            std::fclose(file);
            romfsUnmount("font");

            throw love::Exception("Failed to open font file: {:s}", path);
            return nullptr;
        }

        std::fseek(file, 0, SEEK_END);
        size = std::ftell(file);
        std::rewind(file);

        try
        {
            data = std::make_unique<uint8_t[]>(size);
        }
        catch (std::bad_alloc&)
        {
            throw love::Exception(E_OUT_OF_MEMORY);
        }

        std::fread(data.get(), 1, size, file);
        std::fclose(file);

        romfsUnmount("font");

        uint32_t fontSize = *(uint32_t*)data.get() >> 0x08;
        font              = (CFNT_s*)linearAlloc(fontSize);

        if (font == nullptr)
            throw love::Exception(E_OUT_OF_MEMORY);

        if (font && !decompress_LZ11(font, fontSize, nullptr, data.get() + 0x04, size - 0x04))
        {
            linearFree(font);
            throw love::Exception("Failed to decompress font file: {:s}", path);
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

    CFNT_s* FontModule::loadSystemFont(CFG_Region region, size_t& size)
    {
        const auto index   = getFontIndex(region);
        const auto archive = FontModule::FONT_ARCHIVE_TITLE | (index << 8);

        return loadFromArchive(archive, FontModule::fontPaths[index], size);
    }

    FontModule::FontModule() : FontModuleBase("love.font.ctr")
    {
        this->defaultFontData.set(new SystemFont(CFG_REGION_USA), Acquire::NO_RETAIN);
    }

    Rasterizer* FontModule::newRasterizer(FileData* data) const
    {
        if (BCFNTRasterizer::accepts(data))
            return new BCFNTRasterizer(data, 12);

        throw love::Exception("Invalid font file: {:s}", data->getFilename());
    }

    Rasterizer* FontModule::newTrueTypeRasterizer(Data* data, int size, const Rasterizer::Settings&) const
    {
        return new BCFNTRasterizer(data, size);
    }
} // namespace love
