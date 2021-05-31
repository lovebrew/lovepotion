#pragma once

#include "objects/glyphdata/glyphdata.h"
#include "objects/rasterizer/rasterizer.h"

#include "objects/font/font.h"

#include <citro2d.h>

namespace love
{
    class BCFNTRasterizer : public Rasterizer
    {
      public:
        BCFNTRasterizer(Data* data, int size);

        BCFNTRasterizer(common::Font::SystemFontType type, int size);

        virtual ~BCFNTRasterizer();

        int GetLineHeight() const override
        {
            return 1;
        }

        float GetScale() const
        {
            return this->scale;
        }

        C2D_Font GetFont() const
        {
            return this->font;
        }

        GlyphData* GetGlyphData(uint32_t glyph) const override;

        int GetGlyphCount() const override;

        bool HasGlyph(uint32_t glyph) const override;

        float GetKerning(uint32_t leftGlyph, uint32_t rightGlyph) const override;

        DataType GetDataType() const override
        {
            return DATA_BCFNT;
        }

        static bool Accepts(Data* data);

      private:
        C2D_Font font;
        float scale;

        StrongReference<Data> data;

        mutable int glyphCount;

        void InitMetrics(int size);
    };
} // namespace love
