#pragma once

#include <modules/font/fontmodule.tcc>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <objects/data/filedata/filedata.hpp>
#include <objects/rasterizer_ext.hpp>

#include <3ds.h>

namespace love
{
    class SystemFont : public Data
    {
      public:
        SystemFont(CFG_Region region = CFG_REGION_USA)
        {
            // this->font = FontModule<Console::CTR>::LoadSystemFont(region);
        }

        SystemFont* Clone() const override
        {
            return new SystemFont(*this);
        }

        size_t GetSize() const override
        {
            return 0;
        }

        void* GetData() const override
        {
            return nullptr;
        }

        static inline Type type = Type("SystemFont", &Object::type);

        ~SystemFont()
        {}
    };

    template<>
    class FontModule<Console::CTR> : public FontModule<Console::ALL>
    {
      public:
        static constexpr auto FONT_ARCHIVE = 0x0004009B00014002ULL;

        FontModule();

        virtual ~FontModule()
        {}

        static CFNT_s* LoadSystemFont(CFG_Region region);

        static CFNT_s* LoadFontFromFile(const void* data, size_t size);

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

      private:
        static inline std::array<const char*, 0x04> fontPaths = {
            "font:/cbf_std.bcfnt.lz",
            "font:/cbf_zh-Hans-CN.bcfnt.lz",
            "font:/cbf_ko-Hang-KR.bcfnt.lz",
            "font:/cbf_zh-Hant-TW.bcfnt.lz",
        };
    };
} // namespace love
