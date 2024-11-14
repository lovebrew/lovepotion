#include "modules/font/freetype/Font.hpp"

#include "modules/font/freetype/TrueTypeRasterizer.hpp"
#include "modules/window/Window.hpp"

#include <memory>

namespace love
{
#if defined(__SWITCH__)
    ByteData* FontModule::loadSystemFontByType(SystemFontType type = PlSharedFontType_Standard)
    {
        PlFontData data {};
        if (R_FAILED(plGetSharedFontByType(&data, type)))
            throw love::Exception("Failed to load Shared Font {:d}", (int)type);

        std::string_view name {};
        FontModule::getConstant(type, name);

        if (data.address == nullptr)
            throw love::Exception("Error loading system font '{:s}'", name);

        // create a copy of the data
        return new ByteData(data.address, data.size, false);
    }
#elif defined(__WIIU__)
    ByteData* FontModule::loadSystemFontByType(SystemFontType type = OS_SHAREDDATATYPE_FONT_STANDARD)
    {
        void* data  = nullptr;
        size_t size = 0;

        std::string_view name {};
        FontModule::getConstant(type, name);

        if (!OSGetSharedData(type, 0, &data, &size))
            throw love::Exception("Error loading system font '{:s}'", name);

        return new ByteData(data, size, false);
    }
#else
    #error "Unsupported platform for FreeType"
#endif

    FontModule::FontModule() : FontModuleBase("love.font.freetype")
    {
        if (FT_Init_FreeType(&this->library) != 0)
            throw love::Exception("Error initializing FreeType library.");

        this->defaultFontData.set(loadSystemFontByType(), Acquire::NO_RETAIN);
    }

    FontModule::~FontModule()
    {
        FT_Done_FreeType(this->library);
    }

    Rasterizer* FontModule::newRasterizer(FileData* data) const
    {
        if (TrueTypeRasterizer::accepts(this->library, data))
            return newTrueTypeRasterizer(data, 12, Rasterizer::Settings());

        throw love::Exception("Invalid font file: {:s}", data->getFilename());
    }

    Rasterizer* FontModule::newTrueTypeRasterizer(Data* data, int size,
                                                  const Rasterizer::Settings& settings) const
    {
        float dpiScale = 1.0f;
        auto window    = Module::getInstance<Window>(Module::M_WINDOW);

        if (window != nullptr)
            dpiScale = window->getDPIScale();

        return new TrueTypeRasterizer(this->library, data, size, settings, dpiScale);
    }
} // namespace love
