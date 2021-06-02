#pragma once

#include "common/data.h"
#include "objects/font/fontc.h"

#include <c2d/font.h>
#include <c2d/text.h>

#include "objects/bcfntrasterizer/bcfntrasterizer.h"

enum class love::common::Font::SystemFontType : uint8_t
{
    TYPE_STANDARD  = CFG_REGION_USA,
    TYPE_CHINESE   = CFG_REGION_CHN,
    TYPE_TAIWANESE = CFG_REGION_TWN,
    TYPE_KOREAN    = CFG_REGION_KOR,
    TYPE_MAX_ENUM
};

namespace love
{
    class Font : public love::common::Font
    {
      public:
        static constexpr int FONT_BUFFER_SIZE = 0x200;

        Font(Rasterizer* r, const Texture::Filter& filter);

        virtual ~Font();

        void Print(Graphics* gfx, const std::vector<ColoredString>& text,
                   const Matrix4& localTransform, const Colorf& color) override;

        void Printf(Graphics* gfx, const std::vector<ColoredString>& text, float wrap,
                    AlignMode align, const Matrix4& localTransform, const Colorf& color) override;

        int GetWidth(uint32_t prevGlyph, uint32_t codepoint) override;

        void GetWrap(const std::vector<ColoredString>& text, float wraplimit,
                     std::vector<std::string>& lines, std::vector<int>* lineWidths = nullptr);

        using love::common::Font::GetFilter;

        void SetFilter(const Texture::Filter& filter) override;

        float GetKerning(uint32_t leftGlyph, uint32_t rightGlyph) override;

        float GetKerning(const std::string& leftChar, const std::string& rightChar) override;

        float GetDPIScale() const override;

        float GetAscent() const override;

        float GetBaseline() const override;

        float GetDescent() const override;

        bool HasGlyph(uint32_t glyph) const override;

        void SetFallbacks(const std::vector<Font*>& fallbacks) override;

        using love::common::Font::GetWidth;

        float GetHeight() const override;

        const C2D_Font GetFont();

        void ClearBuffer();

        float GetScale() const;

      private:
        StrongReference<Rasterizer> rasterizer;
        C2D_TextBuf buffer;

        std::unordered_map<uint32_t, float> glyphWidths;
    };
} // namespace love
