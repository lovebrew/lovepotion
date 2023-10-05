#pragma once

#include <common/console.hpp>
#include <common/strongreference.hpp>

#include <objects/rasterizer/rasterizer.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#if defined(__3DS__)
struct FT_Library
{
};

struct FT_Face
{
};
#else
    #include <freetype2/ft2build.h>
    #include FT_FREETYPE_H
    #include FT_GLYPH_H
#endif

namespace love
{
    template<Console::Platform T = Console::ALL>
    class TrueTypeRasterizer : public Rasterizer
    {
      public:
        enum Hinting
        {
            HINTING_NORMAL,
            HINTING_LIGHT,
            HINTING_MONO,
            HINTING_NONE,
            HINTING_MAX_ENUM
        };

        TrueTypeRasterizer(FT_Library library, Data* data, int size, float dpiSacale,
                           Hinting hinting);

        virtual ~TrueTypeRasterizer();

        int GetLineHeight() const override;

        int GetGlyphSpacing(uint32_t glyph) const override;

        int GetGlyphIndex(uint32_t glyph) const override;

        GlyphData* GetGlyphDataForIndex(int index) const override;

        int GetGlyphCount() const override;

        bool HasGlyph(uint32_t glyph) const override;

        float GetKerning(uint32_t left, uint32_t right) const override;

        DataType GetDataType() const override;

        TextShaper* NewTextShaper() override;

        ptrdiff_t GetHandle() const override
        {
            return (ptrdiff_t)face;
        }

        static bool Accepts(FT_Library library, Data* data);

        // clang-format off
        static constexpr BidirectionalMap hintings =
        {
            "normal", HINTING_NORMAL,
            "light",  HINTING_LIGHT,
            "mono",   HINTING_MONO,
            "none",   HINTING_NONE
        };
        // clang-format on

      private:
        FT_Face face;
        StrongReference<Data> data;
        Hinting hinting;
    };
} // namespace love
