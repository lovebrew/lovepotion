#pragma once

#include <common/color.hpp>

#include <objects/imagedata/imagedata.tcc>
#include <objects/rasterizer/rasterizer.hpp>

#include <map>

namespace love
{
    class ImageRasterizer : public Rasterizer
    {
      public:
        ImageRasterizer(ImageData<Console::Which>* data, const uint32_t* glyphs, int glyphCount,
                        int extraSpacing, float dpiScale);

        virtual ~ImageRasterizer()
        {}

        int GetLineHeight() const override;

        int GetGlyphSpacing(uint32_t glyph) const override;

        int GetGlyphIndex(uint32_t glyph) const override;

        GlyphData* GetGlyphDataForIndex(int index) const override;

        int GetGlyphCount() const override;

        bool HasGlyph(uint32_t glyph) const override;

        DataType GetDataType() const override;

        TextShaper* NewTextShaper() override;

      private:
        struct ImageGlyphData
        {
            int x;
            int width;
            uint32_t glyph;
        };

        void Load(const uint32_t* glyphs, int glyphCount);

        StrongReference<ImageData<Console::Which>> imageData;

        int glyphCount;
        int extraSpacing;

        std::vector<ImageGlyphData> imageGlyphs;
        std::map<uint32_t, int> glyphIndicies;

        Color32 spacer;
    };
} // namespace love