#pragma once

#include "common/exception.h"
#include "common/lmath.h"
#include "common/stringmap.h"

#include "common/matrix.h"
#include "common/vector.h"

#include "common/colors.h"
#include "objects/texture/texture.h"

#include "objects/rasterizer/rasterizer.h"

namespace love
{
    class Graphics;

    class Font;
} // namespace love

namespace love::common
{
    class Font : public love::Object
    {
      public:
        struct ColoredString
        {
            std::string string;
            Colorf color;
        };

        enum AlignMode
        {
            ALIGN_LEFT,
            ALIGN_CENTER,
            ALIGN_RIGHT,
            ALIGN_JUSTIFY,
            ALIGN_MAX_ENUM
        };

#if defined(__3DS__)
        static constexpr int MAX_SYSFONTS = 5;
#elif defined(__SWITCH__)
        static constexpr int MAX_SYSFONTS = 7;
#endif

        enum class SystemFontType : uint8_t;

        static love::Type type;

        Font(const Texture::Filter& filter);

        virtual ~Font();

        virtual void Print(Graphics* gfx, const std::vector<ColoredString>& text,
                           const Matrix4& localTransform, const Colorf& color) = 0;

        virtual void Printf(Graphics* gfx, const std::vector<ColoredString>& text, float wrap,
                            AlignMode align, const Matrix4& localTransform,
                            const Colorf& color) = 0;

        int GetWidth(const std::string& string);

        virtual int GetWidth(uint32_t prevGlyph, uint32_t codepoint) = 0;

        virtual float GetHeight() const = 0;

        float GetLineHeight() const;

        void SetLineHeight(float lineHeight);

        Texture::Filter GetFilter();

        virtual void SetFilter(const Texture::Filter& filter) = 0;

        virtual float GetKerning(uint32_t leftGlyph, uint32_t rightGlyph) = 0;

        virtual float GetKerning(const std::string& leftChar, const std::string& rightChar) = 0;

        virtual float GetDPIScale() const = 0;

        virtual float GetAscent() const = 0;

        virtual float GetBaseline() const = 0;

        virtual float GetDescent() const = 0;

        virtual void SetFallbacks(const std::vector<love::Font*>& fallbacks) = 0;

        bool HasGlyphs(const std::string& text) const;

        virtual bool HasGlyph(uint32_t glyph) const = 0;

        static bool GetConstant(const char* in, AlignMode& out);
        static bool GetConstant(AlignMode in, const char*& out);
        static std::vector<const char*> GetConstants(AlignMode);

        static bool GetConstant(const char* in, SystemFontType& out);
        static bool GetConstant(SystemFontType in, const char*& out);
        static std::vector<const char*> GetConstants(SystemFontType);

        static int fontCount;

      protected:
        float lineHeight;
        Texture::Filter filter;

        int height;
        float dpiScale;

        const static StringMap<AlignMode, ALIGN_MAX_ENUM> alignModes;
        const static StringMap<SystemFontType, MAX_SYSFONTS> sharedFonts;
    };
} // namespace love::common
