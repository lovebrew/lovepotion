#pragma once

#include <objects/font/font.tcc>

#include <citro2d.h>

namespace love
{
    template<>
    class Font<Console::CTR> : public Font<Console::ALL>
    {
      public:
        static constexpr int TEXT_BUFFER_SIZE = 0x200;

        Font(Rasterizer<Console::CTR>* rasterizer, const SamplerState& state);

        virtual ~Font();

        int GetWidth(const std::string& text);

        int GetWidth(uint32_t glyph);

        bool HasGlyphs(const std::string& text) const;

        bool HasGlyph(uint32_t glyph) const;

        float GetKerning(uint32_t left, uint32_t right) const;

        float GetKerning(const std::string& left, const std::string& right) const;

        void SetSamplerState(const SamplerState& state);

        void SetFallbacks(const std::vector<Font<Console::CTR>*>& fallbacks)
        {}

        void Print(Graphics<Console::CTR>& graphics, std::vector<ColoredString>& text,
                   const Matrix4<Console::CTR>& localTransform, const Color& color);

        void Printf(Graphics<Console::CTR>& graphics, std::vector<ColoredString>& text, float wrap,
                    AlignMode alignment, const Matrix4<Console::CTR>& localTransform,
                    const Color& color);

        // clang-format off
        static constexpr BidirectionalMap textWrapModes = {
          ALIGN_LEFT,    C2D_AlignLeft,
          ALIGN_CENTER,  C2D_AlignCenter,
          ALIGN_RIGHT,   C2D_AlignRight,
          ALIGN_JUSTIFY, C2D_AlignJustified
        };
        // clang-format on

      private:
        C2D_TextBuf buffer;
        std::unordered_map<uint32_t, float> glyphWidths;
    };
} // namespace love
