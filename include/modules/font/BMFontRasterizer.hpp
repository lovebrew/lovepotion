#pragma once

#include "modules/font/Rasterizer.hpp"
#include "modules/image/ImageData.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace love
{
    class BMFontLine
    {
      public:
        BMFontLine(const std::string& line);

        const std::string& getTag() const
        {
            return this->tag;
        }

        int getAttributeInt(const char* name) const;

        std::string getAttributeString(const char* name) const;

      private:
        std::string tag;
        std::unordered_map<std::string, std::string> attributes;
    };

    class BMFontRasterizer : public Rasterizer
    {
      public:
        BMFontRasterizer(FileData* definition, const std::vector<ImageData*>& images, float dpiScale);

        virtual ~BMFontRasterizer();

        int getLineHeight() const override;

        int getGlyphSpacing(uint32_t glyph) const override;

        int getGlyphIndex(uint32_t glyph) const override;

        GlyphData* getGlyphDataForIndex(int index) const override;

        int getGlyphCount() const override;

        bool hasGlyph(uint32_t glyph) const override;

        float getKerning(uint32_t left, uint32_t right) const override;

        DataType getDataType() const override;

        TextShaper* newTextShaper() override;

        static bool accepts(FileData* definitions);

      private:
        struct BMFontCharacter
        {
            int x;
            int y;
            int page;
            GlyphMetrics metrics;
            uint32_t glyph;
        };

        void parseConfig(const std::string& config);

        std::string folder;
        std::unordered_map<int, StrongRef<ImageData>> images;
        std::vector<BMFontCharacter> characters;
        std::unordered_map<uint32_t, int> indices;
        std::unordered_map<uint64_t, int> kerning;

        int size;
        bool unicode;
        int lineHeight;
    };
} // namespace love
