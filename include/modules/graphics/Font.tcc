#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <stddef.h>

#include "common/Matrix.hpp"
#include "common/Object.hpp"
#include "common/StrongRef.hpp"
#include "common/Vector.hpp"

#include "modules/font/Rasterizer.hpp"
#include "modules/font/TextShaper.hpp"

#include "modules/graphics/Texture.tcc"
#include "modules/graphics/Volatile.hpp"
#include "modules/graphics/vertex.hpp"

namespace love
{
    class GraphicsBase;

    class FontBase : public Object, public Volatile
    {
      public:
        static inline Type type = Type("Font", &Object::type);

        using Codepoints  = std::vector<uint32_t>;
        using GlyphVertex = XYf_STf_RGBAf;

        enum AlignMode
        {
            ALIGN_LEFT,
            ALIGN_CENTER,
            ALIGN_RIGHT,
            ALIGN_JUSTIFY,
            ALIGN_MAX_ENUM
        };

        struct DrawCommand
        {
            TextureBase* texture;
            int startVertex;
            int vertexCount;
        };

        FontBase(Rasterizer* rasterizer, const SamplerState& samplerState);

        virtual ~FontBase();

        std::vector<DrawCommand> generateVertices(const ColoredCodepoints& codepoints, Range range,
                                                  const Color& constantColor,
                                                  std::vector<GlyphVertex>& vertices,
                                                  float extraSpacing = 0.0f, Vector2 offset = {},
                                                  TextShaper::TextInfo* info = nullptr);

        std::vector<DrawCommand> generateVerticesFormatted(const ColoredCodepoints& text,
                                                           const Color& constantColor, float wrap,
                                                           AlignMode align,
                                                           std::vector<GlyphVertex>& vertices,
                                                           TextShaper::TextInfo* info = nullptr);

        void print(GraphicsBase* graphics, const std::vector<ColoredString>& text, const Matrix4& matrix,
                   const Color& color);

        void printf(GraphicsBase* gfx, const std::vector<ColoredString>& text, float wrap, AlignMode align,
                    const Matrix4& m, const Color& constantColor);

        float getHeight() const;

        int getWidth(const std::string& text);

        int getWidth(uint32_t glyph);

        void getWrap(const std::vector<ColoredString>& text, float wraplimit, std::vector<std::string>& lines,
                     std::vector<int>* line_widths = nullptr);

        void getWrap(const ColoredCodepoints& codepoints, float wraplimit, std::vector<Range>& ranges,
                     std::vector<int>* line_widths = nullptr);

        void setLineHeight(float height);

        float getLineHeight() const;

        void setSamplerState(const SamplerState& state);

        const SamplerState& getSamplerState() const;

        int getAscent() const;

        int getDescent() const;

        int getBaseline() const;

        bool hasGlyph(uint32_t glyph) const;

        bool hasGlyphs(const std::string& text) const;

        float getKerning(uint32_t leftglyph, uint32_t rightglyph);

        float getKerning(const std::string& leftchar, const std::string& rightchar);

        void setFallbacks(const std::vector<FontBase*>& fallbacks);

        float getDPIScale() const;

        uint32_t getTextureCacheID() const;

        void unloadVolatile() override;

        // clang-format off
        STRINGMAP_DECLARE(AlignModes, AlignMode,
            { "left",    ALIGN_LEFT    },
            { "center",  ALIGN_CENTER  },
            { "right",   ALIGN_RIGHT   },
            { "justify", ALIGN_JUSTIFY }
        );
        // clang-format on

        struct Glyph
        {
            TextureBase* texture;
            GlyphVertex vertices[4];
        };

        static int fontCount;

      protected:
        static inline uint64_t packGlyphIndex(TextShaper::GlyphIndex glyphindex)
        {
            return ((uint64_t)glyphindex.rasterizerIndex << 32) | (uint64_t)glyphindex.index;
        }

        static inline TextShaper::GlyphIndex unpackGlyphIndex(uint64_t packed)
        {
            return { (int)(packed & 0xFFFFFFFF), (int)(packed >> 32) };
        }

        struct TextureSize
        {
            int width;
            int height;
        };

        TextureSize getNextTextureSize() const;

        GlyphData* getRasterizerGlyphData(TextShaper::GlyphIndex glyphindex, float& dpiscale);

        const Glyph& findGlyph(TextShaper::GlyphIndex glyphindex);

        void printv(GraphicsBase* gfx, const Matrix4& t, const std::vector<DrawCommand>& drawcommands,
                    const std::vector<GlyphVertex>& vertices);

        virtual const Glyph& addGlyph(TextShaper::GlyphIndex glyphindex);

        StrongRef<TextShaper> shaper;

        int textureWidth;
        int textureHeight;

        std::vector<StrongRef<TextureBase>> textures;
        std::unordered_map<uint64_t, Glyph> glyphs;

        PixelFormat pixelFormat;
        SamplerState samplerState;

        float dpiScale;

        int textureX;
        int textureY;

        int rowHeight;

        uint32_t textureCacheID;

        static constexpr int TEXTURE_PADDING = 2;

        virtual void createTexture() = 0;
    };
} // namespace love
