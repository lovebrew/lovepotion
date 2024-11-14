#pragma once

#include "common/Color.hpp"
#include "common/Object.hpp"
#include "common/Range.hpp"
#include "common/StrongRef.hpp"
#include "common/Vector.hpp"
#include "common/int.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace love
{
    class Rasterizer;

    struct ColoredString
    {
        std::string string;
        Color color;
    };

    struct IndexedColor
    {
        Color color;
        int index;
    };

    struct ColoredCodepoints
    {
        std::vector<uint32_t> codepoints;
        std::vector<IndexedColor> colors;
    };

    void getCodepointsFromString(const std::string& string, std::vector<uint32_t>& codepoints);

    void getCodepointsFromString(const std::vector<ColoredString>& strings, ColoredCodepoints& codepoints);

    using StrongRasterizers = std::vector<StrongRef<Rasterizer>>;

    class TextShaper : public Object
    {
      public:
        struct GlyphIndex
        {
            int index;
            int rasterizerIndex;
        };

        using GlyphAdvances = std::unordered_map<uint32_t, std::pair<int, GlyphIndex>>;

        struct GlyphPosition
        {
            Vector2 position;
            GlyphIndex glyphIndex;
        };

        struct TextInfo
        {
            int width;
            int height;
        };

        static constexpr int SPACES_PER_TAB = 4;

        static Type type;

        virtual ~TextShaper();

        const StrongRasterizers& getRasterizers() const
        {
            return this->rasterizers;
        }

        bool isUsingSpacesForTab() const
        {
            return this->useSpacesForTab;
        }

        float getHeight() const
        {
            return this->height;
        }

        void setLineHeight(float height)
        {
            this->lineHeight = height;
        }

        float getLineHeight() const
        {
            return this->lineHeight;
        }

        int getAscent() const;

        int getDescent() const;

        float getBaseline() const;

        bool hasGlyph(uint32_t codepoint) const;

        bool hasGlyphs(const std::string& text) const;

        float getKerning(uint32_t left, uint32_t right);

        float getKerning(const std::string& left, const std::string& right);

        float getGlyphAdvance(uint32_t codepoint, GlyphIndex* glyphIndex = nullptr);

        int getWidth(const std::string& text);

        void getWrap(const std::vector<ColoredString>& text, float limit, std::vector<std::string>& lines,
                     std::vector<int>* lineWidths = nullptr);

        void getWrap(const ColoredCodepoints& codepoints, float limit, std::vector<Range>& lineRanges,
                     std::vector<int>* lineWidths = nullptr);

        virtual void setFallbacks(const std::vector<Rasterizer*>& fallbacks);

        virtual void computeGlyphPositions(const ColoredCodepoints& codepoints, Range range, Vector2 offset,
                                           float extraSpacing, std::vector<GlyphPosition>* positions,
                                           std::vector<IndexedColor>* colors, TextInfo* info) = 0;

        virtual int computeWordWrapIndex(const ColoredCodepoints& codepoints, Range range, float wraplimit,
                                         float* width) = 0;

      protected:
        TextShaper(Rasterizer* rasterizer);

        static inline bool isWhitespace(uint32_t codepoint)
        {
            return codepoint == ' ' || codepoint == '\t';
        }

        StrongRasterizers rasterizers;
        std::vector<float> dpiScales;

      private:
        int height;
        float lineHeight;
        bool useSpacesForTab;

        GlyphAdvances glyphAdvances;
        std::unordered_map<uint64_t, float> kernings;
    };
} // namespace love
