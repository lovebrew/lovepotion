#pragma once

#include <objects/font/font.tcc>

#include <modules/graphics_ext.hpp>
#include <objects/rasterizer_ext.hpp>

#include <utilities/driver/vertex_ext.hpp>

namespace love
{
    template<>
    class Font<Console::CAFE> : public Font<Console::ALL>
    {
      public:
        struct DrawCommand
        {
            Texture<Console::CAFE>* texture;
            int start;
            int count;
        };

        Font(Rasterizer<Console::CAFE>* rasterizer, const SamplerState& state);

        virtual ~Font()
        {}

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

        void SetSamplerState(const SamplerState& state);

        void SetFallbacks(const std::vector<Font<Console::CAFE>*>& fallbacks)
        {}

        void Print(Graphics<Console::CAFE>& graphics, const ColoredStrings& text,
                   const Matrix4<Console::CAFE>& localTransform, const Color& color);

        void Printf(Graphics<Console::CAFE>& graphics, const ColoredStrings& text, float wrap,
                    AlignMode alignment, const Matrix4<Console::CAFE>& localTransform,
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

        float GetBaseline() const
        {
            float ascent        = this->GetAscent();
            const auto truetype = Rasterizer<Console::Which>::DATA_TRUETYPE;

            if (ascent != 0.0f)
                return ascent;
            else if (this->rasterizers[0]->GetDataType() == truetype)
                return std::floor(this->GetHeight() / 1.25f + 0.5f);

            return 0.0f;
        }

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

        uint32_t GetTextureCacheID() const
        {
            return this->textureCacheID;
        }

      private:
        struct Glyph
        {
            Texture<Console::CAFE>* texture;
            int spacing;
            vertex::Vertex vertices[4];
        };

        struct TextureSize
        {
            int width;
            int height;
        };

        static constexpr int TEXTURE_PADDING = 2;
        static constexpr int SPACES_PER_TAB  = 4;

        static constexpr int MAX_TEXTURE_SIZE = 2048;

        static constexpr uint32_t TAB_GLYPH   = 9;
        static constexpr uint32_t SPACE_GLYPH = 32;

        static constexpr uint32_t NEWLINE_GLYPH  = 10;
        static constexpr uint32_t CARRIAGE_GLYPH = 13;

        bool LoadVolatile();

        void UnloadVolatile();

        void CreateTexture();

        TextureSize GetNextTextureSize() const;

        GlyphData* GetRasterizerGlyphData(uint32_t glyph, float& dpiScale);

        const Glyph& AddGlyph(uint32_t glyph);

        const Glyph& FindGlyph(uint32_t glyph);

        void Printv(Graphics<Console::CAFE>& graphics, const Matrix4<Console::CAFE>& transform,
                    const std::vector<DrawCommand>& drawCommands,
                    const std::vector<vertex::Vertex>& vertices);

        int textureX;
        int textureY;
        int textureWidth;
        int textureHeight;

        int rowHeight;

        uint32_t textureCacheID;

        std::vector<StrongReference<Texture<Console::CAFE>>> textures;

        std::unordered_map<uint32_t, Glyph> glyphs;
        std::unordered_map<uint64_t, float> kernings;

        std::vector<StrongRasterizer> rasterizers;
    };
} // namespace love
