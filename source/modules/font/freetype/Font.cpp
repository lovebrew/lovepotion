#include "modules/font/freetype/Font.hpp"

#include "modules/font/freetype/TrueTypeRasterizer.hpp"
#include "modules/window/Window.hpp"

#include "modules/font/SystemFont.hpp"

namespace love
{
    FontModule::FontModule() : FontModuleBase("love.font.freetype")
    {
        if (FT_Init_FreeType(&this->library) != 0)
            throw love::Exception("Error initializing FreeType library.");

        this->defaultFontData.set(new SystemFont(), Acquire::NO_RETAIN);
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
