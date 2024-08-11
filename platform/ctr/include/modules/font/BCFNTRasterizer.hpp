#pragma once

#include "modules/font/Rasterizer.hpp"

#include <3ds.h>

#include <unordered_map>

namespace love
{
    class BCFNTRasterizer : public Rasterizer
    {
      public:
        static bool accepts(Data* data);

        static Type type;

        BCFNTRasterizer(Data* data, int size);

        ~BCFNTRasterizer();

        TextShaper* newTextShaper() override;

        int getLineHeight() const
        {
            return 1;
        }

        bool hasGlyph(uint32_t) const override;

        int getGlyphSpacing(uint32_t glyph) const override;

        int getGlyphIndex(uint32_t) const override;

        GlyphData* getGlyphDataForIndex(int index) const override;

        int getGlyphCount() const override;

        float getKerning(uint32_t, uint32_t) const override
        {
            return 0.0f;
        }

        DataType getDataType() const override
        {
            return DATA_BCFNT;
        }

        ptrdiff_t getHandle() const override;

      private:
        CFNT_s* getUserdata(Data* data) const;

        int glyphCount;
        float scale;

        CFNT_s* userdata;
    };
} // namespace love
