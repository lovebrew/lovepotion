#pragma once

#include <modules/font/fontmodule.tcc>

#include <objects/truetyperasterizer.hpp>
#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

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

        ~FontModule();

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

      private:
        FT_Library library;
    };
} // namespace love
