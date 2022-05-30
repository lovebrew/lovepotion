#pragma once

#include "common/data.h"
#include "objects/font/fontc.h"

#include <c2d/font.h>
#include <c2d/text.h>

#include "objects/bcfntrasterizer/bcfntrasterizer.h"

namespace love
{
    class Font : public love::common::Font
    {
      public:
        static constexpr int FONT_BUFFER_SIZE = 0x200;

        enum class SystemFontType : uint8_t
        {
            TYPE_STANDARD  = CFG_REGION_USA,
            TYPE_CHINESE   = CFG_REGION_CHN,
            TYPE_TAIWANESE = CFG_REGION_TWN,
            TYPE_KOREAN    = CFG_REGION_KOR,
            TYPE_MAX_ENUM
        };

        Font(Rasterizer* rasterizer, const SamplerState& state);

        virtual ~Font();

        void Print(Graphics* gfx, const std::vector<ColoredString>& text,
                   const Matrix4& localTransform, const Colorf& color) override;

        void Printf(Graphics* gfx, const std::vector<ColoredString>& text, float wrap,
                    AlignMode align, const Matrix4& localTransform, const Colorf& color) override;

        int GetWidth(uint32_t prevGlyph, uint32_t codepoint) override;

        void GetWrap(const std::vector<ColoredString>& text, float wraplimit,
                     std::vector<std::string>& lines, std::vector<int>* lineWidths = nullptr);

        void SetSamplerState(const SamplerState& state) override;

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

        static bool GetConstant(const char* in, SystemFontType& out);
        static bool GetConstant(SystemFontType in, const char*& out);
        static std::vector<const char*> GetConstants(SystemFontType);

      private:
        C2D_TextBuf buffer;

        std::unordered_map<uint32_t, float> glyphWidths;
    };
} // namespace love
