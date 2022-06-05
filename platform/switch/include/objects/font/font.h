#pragma once

#include "common/render/vertex.h"
#include "objects/font/fontc.h"

#include "objects/image/image.h"
#include "objects/texture/texture.h"

#include "objects/glyphdata/glyphdata.h"
#include "objects/truetyperasterizer/truetyperasterizer.h"

namespace love
{
    class Font : public love::common::Font
    {
      public:
        enum class SystemFontType : uint8_t
        {
            TYPE_STANDARD               = PlSharedFontType_Standard,
            TYPE_CHINESE_SIMPLIFIED     = PlSharedFontType_ChineseSimplified,
            TYPE_CHINESE_TRADITIONAL    = PlSharedFontType_ChineseTraditional,
            TYPE_CHINESE_SIMPLIFIED_EXT = PlSharedFontType_ExtChineseSimplified,
            TYPE_KOREAN                 = PlSharedFontType_KO,
            TYPE_NINTENDO_EXTENDED      = PlSharedFontType_NintendoExt,
            TYPE_MAX_ENUM
        };

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
                                                  std::vector<Vertex::GlyphVertex>& glyphVertices,
                                                  float extra_spacing = 0.0f, Vector2 offset = {},
                                                  TextInfo* info = nullptr);

        std::vector<DrawCommand> GenerateVerticesFormatted(
            const ColoredCodepoints& text, const Colorf& constantColor, float wrap, AlignMode align,
            std::vector<Vertex::GlyphVertex>& vertices, TextInfo* info = nullptr);

        void Print(Graphics* gfx, const std::vector<ColoredString>& text,
                   const Matrix4& localTransform, const Colorf& color) override;

        void Printf(Graphics* gfx, const std::vector<ColoredString>& text, float wrap,
                    AlignMode align, const Matrix4& localTransform, const Colorf& color) override;

        void PrintV(Graphics* gfx, const Matrix4& t, const std::vector<DrawCommand>& drawcommands,
                    const std::vector<Vertex::GlyphVertex>& vertices);

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
            Vertex::GlyphVertex vertices[4];
        };

        uint32_t GetTextureCacheID();

        Font(Rasterizer* r, const SamplerState& state);

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

        virtual void SetSamplerState(const SamplerState& state) override;

        static bool GetConstant(const char* in, SystemFontType& out);
        static bool GetConstant(SystemFontType in, const char*& out);
        static std::vector<const char*> GetConstants(SystemFontType);

      private:
        struct TextureSize
        {
            int width;
            int height;
        };

        TextureSize GetNextTextureSize() const;

        int textureWidth;
        int textureHeight;

        bool useSpacesAsTab;

        uint32_t textureCacheID;

        int textureX;
        int textureY;
        int rowHeight;

        static constexpr int TEXTURE_PADDING = 2;
        static constexpr int SPACES_PER_TAB  = 4;

        std::unordered_map<uint32_t, Font::Glyph> glyphs;
        std::vector<love::StrongReference<love::Image>> images;
        std::unordered_map<uint64_t, float> kerning;

        void CreateTexture();
    };
} // namespace love
