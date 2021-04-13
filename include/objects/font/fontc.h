#pragma once

#include "common/exception.h"
#include "common/lmath.h"
#include "common/stringmap.h"

#include "common/matrix.h"
#include "common/vector.h"

#include "common/colors.h"
#include "objects/texture/texture.h"

namespace love
{
    class Graphics;
}

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

#if defined(_3DS)
        static constexpr int MAX_SYSFONTS = 5;
#elif defined(__SWITCH__)
        static constexpr int MAX_SYSFONTS = 7;
#endif

        enum class SystemFontType : uint8_t;

        static love::Type type;

        Font();

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

        static Font* GetSystemFontByType(int size, SystemFontType type,
                                         const Texture::Filter& filter = Texture::defaultFilter);

        static bool GetConstant(const char* in, AlignMode& out);
        static bool GetConstant(AlignMode in, const char*& out);
        static std::vector<std::string> GetConstants(AlignMode);

        static bool GetConstant(const char* in, SystemFontType& out);
        static bool GetConstant(SystemFontType in, const char*& out);
        static std::vector<std::string> GetConstants(SystemFontType);

        static int fontCount;

      protected:
        float lineHeight;
        Texture::Filter filter;

        static StringMap<AlignMode, ALIGN_MAX_ENUM>::Entry alignModeEntries[];
        static StringMap<AlignMode, ALIGN_MAX_ENUM> alignModes;

        static StringMap<SystemFontType, MAX_SYSFONTS>::Entry sharedFontEntries[];
        static StringMap<SystemFontType, MAX_SYSFONTS> sharedFonts;
    };
} // namespace love::common