#pragma once

#include <modules/font/fontmodule.tcc>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <objects/data/filedata/filedata.hpp>

#include <coreinit/memory.h>

namespace love
{
    using SystemFontType = OSSharedDataType;

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

        using FontModule<Console::ALL>::NewTrueTypeRasterizer;

        Rasterizer* NewTrueTypeRasterizer(int size, TrueTypeRasterizer<>::Hinting hinting,
                                          OSSharedDataType type) const;

        Rasterizer* NewTrueTypeRasterizer(Data* data, int size,
                                          TrueTypeRasterizer<>::Hinting hinting) const override;

        Rasterizer* NewTrueTypeRasterizer(Data* data, int size, float dpiScale,
                                          TrueTypeRasterizer<>::Hinting hinting) const override;

        Rasterizer* NewRasterizer(FileData* data) const;

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
