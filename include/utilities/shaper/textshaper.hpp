#pragma once

#include <common/color.hpp>
#include <common/object.hpp>
#include <common/range.hpp>
#include <common/vector.hpp>

#include <common/strongreference.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace love
{
    class Rasterizer;

    struct ColoredString
    {
        std::string str;
        Color color;
    };

    struct IndexedColor
    {
        Color color;
        int index;
    };

    struct ColoredCodepoints
    {
        std::vector<uint32_t> cps;
        std::vector<IndexedColor> colors;
    };

    void GetCodepointsFromString(const std::string& string, std::vector<uint32_t>& codepoints);

    void GetCodepointsFromString(const std::vector<ColoredString>& strings,
                                 ColoredCodepoints& codepoints);

    class TextShaper : public Object
    {
      public:
        struct GlyphIndex
        {
            int index;
            int rasterizerIndex;
        };

        struct GlyphPosition
        {
            Vector2 position;
            GlyphIndex glyphIndex;
        };

        struct TextInfo
        {
            float width;
            float height;
        };

        static constexpr int SPACES_PER_TAB = 4;

        static love::Type type;

        virtual ~TextShaper()
        {}

        const std::vector<StrongReference<Rasterizer>>& GetRasterizers() const
        {
            return this->rasterizers;
        }

        bool IsUsingSpacesForTab() const
        {
            return this->useSpacesForTab;
        }

        float GetHeight() const
        {
            return this->height;
        }

        void SetLineHeight(float lineHeight)
        {
            this->lineHeight = lineHeight;
        }

        float GetLineHeight() const
        {
            return this->lineHeight;
        }

        int GetAscent() const;

        int GetDescent() const;

        float GetBaseline() const;

        bool HasGlyph(uint32_t glyph) const;

        bool HasGlyphs(const std::string& text) const;

        float GetKerning(uint32_t leftGlyph, uint32_t rightGlyph);

        float GetKerning(const std::string& left, const std::string& right);

        int GetGlyphAdvance(uint32_t glyph, GlyphIndex* index = nullptr);

        int GetWidth(const std::string& string);

        void GetWrap(const std::vector<ColoredString>& text, float wrapLimit,
                     std::vector<std::string>& lines, std::vector<int>* lineWidths = nullptr);

        void GetWrap(const ColoredCodepoints& codepoints, float wrapLimit,
                     std::vector<Range>& lineRanges, std::vector<int>* lineWidths = nullptr);

        virtual void SetFallbacks(const std::vector<Rasterizer*>& fallbacks);

        virtual void ComputeGlyphPositions(const ColoredCodepoints& codepoints, Range range,
                                           Vector2 offset, float extraSpacing,
                                           std::vector<GlyphPosition>* positions,
                                           std::vector<IndexedColor>* colors, TextInfo* info) = 0;

        virtual int ComputeWordWrapIndex(const ColoredCodepoints& codepoints, Range range,
                                         float wrapLimit, float* width) = 0;

      protected:
        TextShaper(Rasterizer* rasterizer);

        static inline bool IsWhitespace(uint32_t codepoint)
        {
            return codepoint == ' ' || codepoint == '\t';
        }

        std::vector<StrongReference<Rasterizer>> rasterizers;
        std::vector<float> dpiScales;

      private:
        int height;
        float lineHeight;
        bool useSpacesForTab;

        std::unordered_map<uint32_t, std::pair<int, GlyphIndex>> glyphAdvances;
        std::unordered_map<uint64_t, float> kernings;
    };
} // namespace love