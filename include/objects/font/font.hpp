#pragma once

#include <common/color.hpp>
#include <common/console.hpp>
#include <common/exception.hpp>
#include <common/math.hpp>
#include <common/strongreference.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <utilities/driver/renderer/renderstate.hpp>
#include <utilities/driver/renderer/samplerstate.hpp>
#include <utilities/driver/renderer/vertex.hpp>

#include <objects/rasterizer_ext.hpp>
#include <objects/texture/texture.tcc>

#include <vector>

#if defined(__3DS__)
    #include <citro3d.h>
using TextureHandle = C3D_Tex;
#else
using TextureHandle = Texture<Console::Which>;
#endif

namespace love
{
    using StrongRasterizer = StrongReference<Rasterizer<Console::Which>>;

    class Font : public Object
    {
      public:
        static inline Type type = Type("Font", &Object::type);

        using Codepoints = std::vector<uint32_t>;

        enum AlignMode
        {
            ALIGN_LEFT,
            ALIGN_CENTER,
            ALIGN_RIGHT,
            ALIGN_JUSTIFY,
            ALIGN_MAX_ENUM
        };

        struct ColoredString
        {
            std::string string;
            Color color;
        };

        using ColoredStrings = std::vector<ColoredString>;

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

        struct TextInfo
        {
            int width;
            int height;
        };

        struct Glyph
        {
            TextureHandle* texture;
            int spacing;
            std::array<vertex::Vertex, 0x06> vertices;

            int sheet;
        };

        struct DrawCommand
        {
            TextureHandle* texture;
            int start;
            int count;

            int sheet;
        };

        static inline int fontCount = 0;

        Font(Rasterizer<Console::Which>* rasterizer, const SamplerState& state);

        virtual ~Font()
        {
            Font::fontCount--;
        }

        int GetWidth(std::string_view text);

        int GetWidth(uint32_t glyph);

        bool HasGlyphs(std::string_view text) const;

        bool HasGlyph(uint32_t glyph) const;

        float GetKerning(uint32_t left, uint32_t right);

        float GetKerning(std::string_view left, std::string_view right);

        static void GetCodepointsFromString(std::string_view text, Codepoints& codepoints);

        static void GetCodepointsFromString(const ColoredStrings& strings,
                                            ColoredCodepoints& codepoints);

        void GetWrap(const std::vector<ColoredString>& text, float wraplimit,
                     std::vector<std::string>& lines, std::vector<int>* line_widths = nullptr);

        void GetWrap(const ColoredCodepoints& codepoints, float wraplimit,
                     std::vector<ColoredCodepoints>& lines, std::vector<int>* linewidths = nullptr);

        void SetFallbacks(const std::vector<Font*>& fallbacks)
        {}

        void Print(Graphics<Console::Which>& graphics, const ColoredStrings& text,
                   const Matrix4<Console::Which>& localTransform, const Color& color);

        void Printf(Graphics<Console::Which>& graphics, const ColoredStrings& text, float wrap,
                    AlignMode alignment, const Matrix4<Console::Which>& localTransform,
                    const Color& color);

        int GetAscent() const
        {
            return std::floor(this->rasterizers[0]->GetAscent() / this->dpiScale + 0.5f);
        }

        int GetDescent() const
        {
            return std::floor(this->rasterizers[0]->GetDescent() / this->dpiScale + 0.5f);
        }

        float GetHeight() const
        {
            return std::floor(this->height / this->dpiScale + 0.5f);
        }

        uint32_t GetTextureCacheID() const
        {
            return this->textureCacheID;
        }

        float GetBaseline() const;

        std::vector<DrawCommand> GenerateVertices(const ColoredCodepoints& codepoints,
                                                  const Color& color,
                                                  std::vector<vertex::Vertex>& vertices,
                                                  float extraSpacing = 0.0f, Vector2 offset = {},
                                                  TextInfo* info = nullptr);

        std::vector<DrawCommand> GenerateVerticesFormatted(const ColoredCodepoints& codepoints,
                                                           const Color& color, float wrap,
                                                           AlignMode align,
                                                           std::vector<vertex::Vertex>& vertices,
                                                           TextInfo* info = nullptr);

        void SetFont(Font* font);

        void SetLineHeight(float height)
        {
            this->lineHeight = height;
        }

        float GetLineHeight() const
        {
            return this->lineHeight;
        }

        void SetSamplerState(const SamplerState& state);

        const SamplerState& GetSamplerState() const
        {
            return this->samplerState;
        }

        float GetDPIScale() const
        {
            return this->dpiScale;
        }

        // clang-format off
        static constexpr BidirectionalMap alignModes = {
            "left",    ALIGN_LEFT,
            "center",  ALIGN_CENTER,
            "right",   ALIGN_RIGHT,
            "justify", ALIGN_JUSTIFY
        };
        // clang-format on

      private:
        struct TextureSize
        {
            int width;
            int height;
        };

        static constexpr int MAX_TEXTURE_SIZE = 2048;

        bool LoadVolatile();

        void UnloadVolatile();

        void CreateTexture();

        TextureSize GetNextTextureSize() const;

        GlyphData* GetRasterizerGlyphData(uint32_t glyph, float& dpiScale);

        const Glyph& AddGlyph(uint32_t glyph);

        const Glyph& FindGlyph(uint32_t glyph);

        void Printv(Graphics<Console::Which>& graphics, const Matrix4<Console::Which>& transform,
                    const std::vector<DrawCommand>& drawCommands,
                    const std::vector<vertex::Vertex>& vertices);

        int textureX;
        int textureY;
        int textureWidth;
        int textureHeight;

        int rowHeight;

        uint32_t textureCacheID;

#if defined(__3DS__)
        std::vector<C3D_Tex> textures;
#else
        std::vector<StrongReference<Texture<Console::Which>>> textures;
#endif

        std::unordered_map<uint32_t, Glyph> glyphs;
        std::unordered_map<uint64_t, float> kernings;
        std::vector<StrongRasterizer> rasterizers;

        static constexpr auto SPACES_PER_TAB  = 0x04;
        static constexpr uint32_t TAB_GLYPH   = 9;
        static constexpr uint32_t SPACE_GLYPH = 32;

        static constexpr uint32_t NEWLINE_GLYPH  = 10;
        static constexpr uint32_t CARRIAGE_GLYPH = 13;

        float lineHeight;
        float height;

        SamplerState samplerState;
        float dpiScale;
        bool useSpacesAsTab;

        PixelFormat format;
    };
} // namespace love
