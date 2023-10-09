#pragma once

#include <common/console.hpp>
#include <common/module.hpp>

#include <common/data.hpp>
#include <common/strongreference.hpp>

#include <objects/truetyperasterizer/truetyperasterizer.tcc>

#include <objects/imagedata/imagedata.tcc>

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

        virtual ~FontModule();

        virtual ModuleType GetModuleType() const
        {
            return M_FONT;
        }

        virtual const char* GetName() const
        {
            return "love.font";
        }

        Rasterizer* NewBMFontRasterizer(FileData* data,
                                        const std::vector<ImageData<Console::Which>*>& images,
                                        float dpiScale) const;

        Rasterizer* NewImageRasterizer(ImageData<Console::Which>* data, const std::string& text,
                                       int extraSpacing, float dpiScale) const;

        Rasterizer* NewImageRasterizer(ImageData<Console::Which>* data, uint32_t* glyphs,
                                       int glyphCount, int extraSpacing, float dpiScale) const;

        Rasterizer* NewTrueTypeRasterizer(int size, TrueTypeRasterizer<>::Hinting hinting) const;

        Rasterizer* NewTrueTypeRasterizer(int size, float dpiScale,
                                          TrueTypeRasterizer<>::Hinting hinting) const;

        virtual Rasterizer* NewTrueTypeRasterizer(Data* data, int size,
                                                  TrueTypeRasterizer<>::Hinting hinting) const = 0;

        virtual Rasterizer* NewTrueTypeRasterizer(Data* data, int size, float dpiScale,
                                                  TrueTypeRasterizer<>::Hinting hinting) const = 0;

        GlyphData* NewGlyphData(Rasterizer* rasterizer, const std::string& text) const;

        GlyphData* NewGlyphData(Rasterizer* rasterizer, uint32_t glyph) const;

      protected:
        StrongReference<Data> defaultFontData;
        FT_Library library;
    };
} // namespace love
