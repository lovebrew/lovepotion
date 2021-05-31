#pragma once

#include "deko3d/vertex.h"
#include "objects/font/fontc.h"

#include "objects/image/image.h"
#include "objects/texture/texture.h"

#include "objects/glyphdata/glyphdata.h"
#include "objects/truetyperasterizer/truetyperasterizer.h"

enum class love::common::Font::SystemFontType : uint8_t
{
    TYPE_STANDARD               = PlSharedFontType_Standard,
    TYPE_CHINESE_SIMPLIFIED     = PlSharedFontType_ChineseSimplified,
    TYPE_CHINESE_TRADITIONAL    = PlSharedFontType_ChineseTraditional,
    TYPE_CHINESE_SIMPLIFIED_EXT = PlSharedFontType_ExtChineseSimplified,
    TYPE_KOREAN                 = PlSharedFontType_KO,
    TYPE_NINTENDO_EXTENDED      = PlSharedFontType_NintendoExt,
    TYPE_MAX_ENUM
};

namespace love
{
    class Font : public love::common::Font
    {
      public:
        typedef std::vector<uint32_t> Codepoints;

        struct IndexedColor
        {
            Colorf color;
            int index;
        };

        struct ColoredCodepoints
        {
            std::vector<uint32_t> codes;
            std::vector<IndexedColor> colors;
        };

        struct TextInfo
        {
            int width;
            int height;
        };

        static void GetCodepointsFromString(const std::string& string, Codepoints& points);

        static void GetCodepointsFromString(const std::vector<ColoredString>& strings,
                                            ColoredCodepoints& codepoints);

        struct DrawCommand
        {
            int startVertex;
            int vertexCount;

            love::Texture* texture;
        };

        std::vector<DrawCommand> GenerateVertices(const ColoredCodepoints& codepoints,
                                                  const Colorf& constantcolor,
                                                  std::vector<vertex::GlyphVertex>& glyphVertices,
                                                  float extra_spacing = 0.0f, Vector2 offset = {},
                                                  TextInfo* info = nullptr);

        std::vector<DrawCommand> GenerateVerticesFormatted(
            const ColoredCodepoints& text, const Colorf& constantColor, float wrap, AlignMode align,
            std::vector<vertex::GlyphVertex>& vertices, TextInfo* info = nullptr);

        void Print(Graphics* gfx, const std::vector<ColoredString>& text,
                   const Matrix4& localTransform, const Colorf& color) override;

        void Printf(Graphics* gfx, const std::vector<ColoredString>& text, float wrap,
                    AlignMode align, const Matrix4& localTransform, const Colorf& color) override;

        void PrintV(Graphics* gfx, const Matrix4& t, const std::vector<DrawCommand>& drawcommands,
                    const std::vector<vertex::GlyphVertex>& vertices);

        void GetWrap(const std::vector<ColoredString>& text, float wraplimit,
                     std::vector<std::string>& lines, std::vector<int>* lineWidths = nullptr);

        void GetWrap(const ColoredCodepoints& codepoints, float wraplimit,
                     std::vector<ColoredCodepoints>& lines, std::vector<int>* lineWidths = nullptr);

        int GetWidth(uint32_t prevGlyph, uint32_t codepoint) override;

        using common::Font::GetWidth;

        float GetHeight() const override;

        struct Glyph
        {
            love::Texture* texture;
            int spacing;
            vertex::GlyphVertex vertices[4];
        };

        uint32_t GetTextureCacheID();

        Font(Rasterizer* r, const Texture::Filter& filter);

        virtual ~Font();

        const Font::Glyph& FindGlyph(uint32_t glyph);

        const Font::Glyph& AddGlyph(uint32_t glyph);

        bool HasGlyph(uint32_t glyph) const override;

        love::GlyphData* GetRasterizerGlyphData(uint32_t glyph);

        float GetKerning(uint32_t leftGlyph, uint32_t rightGlyph) override;

        float GetKerning(const std::string& leftChar, const std::string& rightChar) override;

        float GetDPIScale() const override;

        void SetFallbacks(const std::vector<Font*>& fallbacks) override;

        float GetAscent() const override;

        float GetBaseline() const override;

        float GetDescent() const override;

        void SetFilter(const Texture::Filter& filter);

      private:
        struct TextureSize
        {
            int width;
            int height;
        };

        TextureSize GetNextTextureSize() const;

        std::vector<love::StrongReference<love::Rasterizer>> rasterizers;

        int textureWidth;
        int textureHeight;

        bool useSpacesAsTab;

        uint32_t textureCacheID;

        int textureX;
        int textureY;
        int rowHeight;

        static const int TEXTURE_PADDING = 2;

        static const int SPACES_PER_TAB = 4;

        std::unordered_map<uint32_t, Font::Glyph> glyphs;

        std::vector<love::StrongReference<love::Image>> images;

        std::unordered_map<uint64_t, float> kerning;

        void CreateTexture();
    };
} // namespace love
