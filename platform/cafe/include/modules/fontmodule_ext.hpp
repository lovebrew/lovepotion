#pragma once

#include <modules/font/fontmodule.tcc>

#include <objects/data/filedata/filedata.hpp>
#include <objects/rasterizer_ext.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <coreinit/memory.h>

namespace love
{
    class SystemFont : public Data
    {
      public:
        SystemFont(OSSharedDataType type = OS_SHAREDDATATYPE_FONT_STANDARD) : size(0), type(type)
        {
            OSGetSharedData(type, 0, &this->data, &this->size);
        }

        ~SystemFont()
        {}

        SystemFont* Clone() const override
        {
            return new SystemFont(this->type);
        }

        void* GetData() const override
        {
            return this->data;
        }

        size_t GetSize() const override
        {
            return this->size;
        }

      private:
        void* data;
        size_t size;

        OSSharedDataType type;
    };

    template<>
    class FontModule<Console::CAFE> : public FontModule<Console::ALL>
    {
      public:
        FontModule();

        ~FontModule();

        Rasterizer* NewTrueTypeRasterizer(int size, OSSharedDataType type,
                                          TrueTypeRasterizer<>::Hinting hinting) const;

        // clang-format off
        static constexpr BidirectionalMap systemFonts = {
          "standard",  OS_SHAREDDATATYPE_FONT_STANDARD,
          "chinese",   OS_SHAREDDATATYPE_FONT_CHINESE,
          "korean",    OS_SHAREDDATATYPE_FONT_KOREAN,
          "taiwanese", OS_SHAREDDATATYPE_FONT_TAIWANESE
        };
        // clang-format on

      private:
        FT_Library library;
    };
} // namespace love
