#pragma once

#include <objects/imagedata/imagedata.tcc>

#include <objects/rasterizer/rasterizer.hpp>

#include <unordered_map>
#include <vector>

namespace love
{
    class BMFontRasterizer : public Rasterizer
    {
      public:
        BMFontRasterizer(FileData* fontDefinition,
                         const std::vector<ImageData<Console::Which>*>& imageList, float dpiScale);

        virtual ~BMFontRasterizer()
        {}

        int GetLineHeight() const override;

        int GetGlyphSpacing(uint32_t glyph) const override;

        int GetGlyphIndex(uint32_t glyph) const override;

        GlyphData* GetGlyphDataForIndex(int index) const override;

        int GetGlyphCount() const override;

        bool HasGlyph(uint32_t glyph) const override;

        float GetKerning(uint32_t left, uint32_t right) const override;

        DataType GetDataType() const override;

        TextShaper* NewTextShaper() override;

        static bool Accepts(FileData* fontDefinition);

      private:
        struct BMFontCharacter
        {
            int x;
            int y;
            int page;
            GlyphData::GlyphMetrics metrics;
            uint32_t glyph;
        };

        void ParseConfig(const std::string& config);

        std::string fontFolder;
        std::unordered_map<int, StrongReference<ImageData<Console::Which>>> images;

        std::vector<BMFontCharacter> characters;

        std::unordered_map<uint64_t, int> characterIndices;
        std::unordered_map<uint64_t, int> kernings;

        int fontSize;
        bool unicode;
        int lineHeight;
    };
} // namespace love