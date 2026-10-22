#pragma once

#include "common/Color.hpp"

#include "modules/font/Rasterizer.hpp"
#include "modules/image/ImageData.hpp"

#include <map>

namespace love
{
    class ImageRasterizer : public Rasterizer
    {
      public:
        ImageRasterizer(ImageData* data, const uint32_t* glyphs, int count, int spacing, float dpiScale);

        virtual ~ImageRasterizer();

        int getLineHeight() const override;

        int getGlyphSpacing(uint32_t glyph) const override;

        int getGlyphIndex(uint32_t glyph) const override;

        GlyphData* getGlyphDataForIndex(int index) const override;

        int getGlyphCount() const override;

        bool hasGlyph(uint32_t glyph) const override;

        DataType getDataType() const override;

        TextShaper* newTextShaper() override;

      private:
        struct ImageGlyphData
        {
            int x;
            int width;
            uint32_t glyph;
        };

        void load(const uint32_t* glyphs, int count);

        StrongRef<ImageData> data;
        int count;
        int spacing;

        std::vector<ImageGlyphData> glyphs;
        std::map<uint32_t, int> indices;

        Color32 spacer;
    };
} // namespace love
