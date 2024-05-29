#pragma once

#include "modules/font/Font.tcc"

#include <3ds.h>

namespace love
{
    class FontModule : public FontModuleBase
    {
      public:
        FontModule();

        static CFNT_s* loadSystemFont(CFG_Region region, size_t& size);

        virtual Rasterizer* newRasterizer(FileData* data) const override;

        Rasterizer* newTrueTypeRasterizer(Data* data, int size,
                                          const Rasterizer::Settings& settings) const override;

        using FontModuleBase::newTrueTypeRasterizer;

      private:
        static constexpr auto FONT_ARCHIVE_TITLE = 0x0004009B00014002ULL;

        static inline std::array<const char*, 0x04> fontPaths = {
            "font:/cbf_std.bcfnt.lz",
            "font:/cbf_zh-Hans-CN.bcfnt.lz",
            "font:/cbf_ko-Hang-KR.bcfnt.lz",
            "font:/cbf_zh-Hant-TW.bcfnt.lz",
        };
    };
} // namespace love
