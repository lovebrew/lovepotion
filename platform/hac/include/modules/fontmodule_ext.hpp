#pragma once

#include <modules/font/fontmodule.tcc>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <objects/data/filedata/filedata.hpp>

#include <switch.h>

namespace love
{
    class SystemFont : public Data
    {
      public:
        SystemFont(PlSharedFontType type = PlSharedFontType_Standard) : type(type)
        {
            plGetSharedFontByType(&this->data, type);
        }

        Data* Clone() const override
        {
            return new SystemFont(this->type);
        }

        void* GetData() const override
        {
            return this->data.address;
        }

        size_t GetSize() const override
        {
            return this->data.size;
        }

      private:
        PlFontData data;
        PlSharedFontType type;
    };

    template<>
    class FontModule<Console::HAC> : public FontModule<Console::ALL>
    {
      public:
        FontModule();

        using FontModule<Console::ALL>::NewTrueTypeRasterizer;

        Rasterizer* NewTrueTypeRasterizer(int size, TrueTypeRasterizer<>::Hinting hinting,
                                          PlSharedFontType type) const;

        Rasterizer* NewTrueTypeRasterizer(Data* data, int size,
                                          TrueTypeRasterizer<>::Hinting hinting) const override;

        Rasterizer* NewTrueTypeRasterizer(Data* data, int size, float dpiScale,
                                          TrueTypeRasterizer<>::Hinting hinting) const override;

        Rasterizer* NewRasterizer(FileData* data) const;

        // clang-format off
        static constexpr BidirectionalMap systemFonts = {
          "standard",                    PlSharedFontType_Standard,
          "chinese simplified",          PlSharedFontType_ChineseSimplified,
          "chinese simplified extended", PlSharedFontType_ExtChineseSimplified,
          "chinese traditional",         PlSharedFontType_ChineseTraditional,
          "korean",                      PlSharedFontType_KO,
          "nintendo extended",           PlSharedFontType_NintendoExt
        };
        // clang-format on
    };
} // namespace love
