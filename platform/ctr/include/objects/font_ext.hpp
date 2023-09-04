#pragma once

#include <objects/font/font.tcc>

#include <objects/rasterizer_ext.hpp>

#include <citro3d.h>

namespace love
{
    template<>
    class Font<Console::CTR> : public Font<Console::ALL>
    {
      public:
        static constexpr int TEXT_BUFFER_SIZE = 0x200;

        Font(Rasterizer<Console::CTR>* rasterizer, const SamplerState& state);

        virtual ~Font();

        int GetWidth(const std::string_view& text);

        int GetWidth(uint32_t glyph);

        bool HasGlyphs(const std::string_view& text) const;

        bool HasGlyph(uint32_t glyph) const;

        float GetKerning(uint32_t left, uint32_t right) const;

        float GetKerning(const std::string& left, const std::string& right) const;

        void SetSamplerState(const SamplerState& state);

        void SetFallbacks(const std::vector<Font<Console::CTR>*>& fallbacks);

        const Glyph<C3D_Tex, 0x06>& AddGlyph(uint32_t glyph);

        std::vector<Font<>::DrawCommand<C3D_Tex>> GenerateVertices(
            const ColoredCodepoints& codepoints, const Color& color,
            std::vector<vertex::Vertex>& vertices, float extraSpacing = 0.0f, Vector2 offset = {},
            TextInfo* info = nullptr);

        std::vector<Font<>::DrawCommand<C3D_Tex>> GenerateVerticesFormatted(
            const ColoredCodepoints& codepoints, const Color& color, float wrap, AlignMode align,
            std::vector<vertex::Vertex>& vertices, TextInfo* info = nullptr);

        const Glyph<C3D_Tex, 0x06>& FindGlyph(uint32_t glyph);

        void Render(Graphics<Console::CTR>& graphics, const Matrix4<Console::CTR>& transform,
                    const std::vector<Font<>::DrawCommand<C3D_Tex>>& commands,
                    const std::vector<vertex::Vertex>& vertices);

        void Print(Graphics<Console::CTR>& graphics, const ColoredStrings& text,
                   const Matrix4<Console::CTR>& localTransform, const Color& color);

        void Printf(Graphics<Console::CTR>& graphics, const ColoredStrings& text, float wrap,
                    AlignMode alignment, const Matrix4<Console::CTR>& localTransform,
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
            return this->height;
        }

        void GetWrap(const std::vector<ColoredString>& text, float wraplimit,
                     std::vector<std::string>& lines, std::vector<int>* line_widths = nullptr);

        void GetWrap(const ColoredCodepoints& codepoints, float wraplimit,
                     std::vector<ColoredCodepoints>& lines, std::vector<int>* linewidths = nullptr);

        float GetBaseline() const
        {
            float ascent = this->GetAscent();

            if (ascent != 0.0f)
                return ascent;
            else if (this->rasterizers[0]->GetDataType() == Rasterizer<>::DATA_TRUETYPE)
                return std::floor(this->GetHeight() / 1.25f + 0.5f);

            return 0.0f;
        }

        GlyphData* GetRasterizerGlyphData(uint32_t glyph)
        {
            if (glyph == Font::TAB_GLYPH && this->useSpacesAsTab)
            {
                auto* space = this->rasterizers[0]->GetGlyphData(Font::SPACE_GLYPH);

                GlyphData::GlyphMetrics metrics {};
                metrics.advance  = space->GetAdvance() * Font::SPACES_PER_TAB;
                metrics.bearingX = space->GetBearingX();
                metrics.bearingY = space->GetBearingY();

                space->Release();
                return new GlyphData(glyph, metrics, space->GetFormat());
            }

            for (const auto& rasterizer : this->rasterizers)
            {
                if (rasterizer->HasGlyph(glyph))
                    return rasterizer->GetGlyphData(glyph);
            }

            return this->rasterizers[0]->GetGlyphData(glyph);
        }

        float GetScale() const
        {
            return this->rasterizers[0]->GetScale();
        }

        Rasterizer<Console::CTR>::GlyphSheetInfo& GetGlyphSheetInfo(uint32_t glyph);

      private:
        bool LoadVolatile();

        void CreateTexture();

        std::unordered_map<uint32_t, Glyph<C3D_Tex, 0x06>> glyphs;
        std::vector<C3D_Tex> textures;
        std::unordered_map<uint32_t, float> glyphWidths;
        std::vector<StrongRasterizer> rasterizers;
    };
} // namespace love
