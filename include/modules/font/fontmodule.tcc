#pragma once

#include <common/console.hpp>
#include <common/module.hpp>

#include <common/data.hpp>
#include <common/strongreference.hpp>

#include <objects/truetyperasterizer/truetyperasterizer.tcc>

#include <objects/data/filedata/filedata.hpp>

#include <memory>

namespace love
{
    class SystemFont;

    template<Console::Platform T = Console::ALL>
    class FontModule : public Module
    {
      public:
        FontModule();

        virtual ~FontModule()
        {}

        virtual ModuleType GetModuleType() const
        {
            return M_FONT;
        }

        virtual const char* GetName() const
        {
            return "love.font";
        }

        Rasterizer* NewTrueTypeRasterizer(int size,
                                          TrueTypeRasterizer<Console::ALL>::Hinting hinting) const;

        Rasterizer* NewTrueTypeRasterizer(int size, float dpiScale,
                                          TrueTypeRasterizer<Console::ALL>::Hinting hinting) const;

        virtual Rasterizer* NewTrueTypeRasterizer(
            Data* data, int size, TrueTypeRasterizer<Console::ALL>::Hinting hinting) const = 0;

        virtual Rasterizer* NewTrueTypeRasterizer(
            Data* data, int size, float dpiScale,
            TrueTypeRasterizer<Console::ALL>::Hinting hinting) const = 0;

        GlyphData* NewGlyphData(Rasterizer* rasterizer, const std::string& glyph) const;

        GlyphData* NewGlyphData(Rasterizer* rasterizer, uint32_t glyph) const;

      protected:
        StrongReference<Data> defaultFontData;
        FT_Library library;
    };
} // namespace love
