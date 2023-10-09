#pragma once

#include <modules/font/fontmodule.tcc>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <objects/data/filedata/filedata.hpp>
#include <objects/rasterizer/rasterizer.hpp>

#include <3ds.h>

namespace love
{
    using SystemFontType = CFG_Region;

    class SystemFont : public Data
    {
      public:
        SystemFont(CFG_Region region = CFG_REGION_USA);

        SystemFont* Clone() const override
        {
            return new SystemFont(*this);
        }

        size_t GetSize() const override
        {
            return this->size;
        }

        void* GetData() const override
        {
            return (void*)this->font;
        }

        static inline Type type = Type("SystemFont", &Object::type);

        ~SystemFont()
        {
            if (this->font)
                linearFree(this->font);
        }

      private:
        CFNT_s* font;
        size_t size;
    };

    template<>
    class FontModule<Console::CTR> : public FontModule<Console::ALL>
    {
      public:
        static constexpr auto FONT_ARCHIVE = 0x0004009B00014002ULL;

        FontModule();

        virtual ~FontModule()
        {}

        static CFNT_s* LoadSystemFont(CFG_Region region, size_t& size);

        using FontModule<Console::ALL>::NewTrueTypeRasterizer;

        Rasterizer* NewRasterizer(FileData* data) const;

        Rasterizer* NewTrueTypeRasterizer(Data* data, int size,
                                          TrueTypeRasterizer<>::Hinting hinting) const override;

        Rasterizer* NewTrueTypeRasterizer(Data* data, int size, float dpiScale,
                                          TrueTypeRasterizer<>::Hinting hinting) const override;

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
