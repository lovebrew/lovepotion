#pragma once

#include <modules/font/fontmodule.tcc>
#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <objects/data/filedata/filedata.hpp>
#include <objects/rasterizer_ext.hpp>

#include <citro2d.h>

#include <3ds.h>

namespace love
{
    class SystemFont : public Data
    {
      public:
        SystemFont(CFG_Region region = CFG_REGION_USA)
        {
            this->font = C2D_FontLoadSystem(region);
        }

        ~SystemFont()
        {
            C2D_FontFree(this->font);
        }

        void* GetData() const override
        {
            return nullptr;
        }

        SystemFont* Clone() const override
        {
            return new SystemFont(*this);
        }

        size_t GetSize() const override
        {
            return 0;
        }

      private:
        C2D_Font font;
    };

    template<>
    class FontModule<Console::CTR> : public FontModule<Console::ALL>
    {
      public:
        FontModule();

        virtual ~FontModule()
        {}

        Rasterizer<Console::CTR>* NewRasterizer(FileData* data) const;

        Rasterizer<Console::CTR>* NewBCFNTRasterizer(Data* data, int size) const;

        Rasterizer<Console::CTR>* NewBCFNTRasterizer(int size);

        Rasterizer<Console::CTR>* NewBCFNTRasterizer(int size,
                                                     CFG_Region region = CFG_REGION_USA) const;

        GlyphData* NewGlyphData(Rasterizer<Console::CTR>* rasterizer,
                                const std::string& text) const;

        GlyphData* NewGlyphData(Rasterizer<Console::CTR>* rasterizer, uint32_t glyph) const;

        // clang-format off
        static constexpr BidirectionalMap systemFonts = {
          "standard",  CFG_REGION_USA,
          "chinese",   CFG_REGION_CHN,
          "taiwanese", CFG_REGION_TWN,
          "korean",    CFG_REGION_KOR
        };
        // clang-format on
    };
} // namespace love
