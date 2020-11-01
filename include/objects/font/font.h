#pragma once

#include "common/exception.h"
#include "common/mmath.h"
#include "common/stringmap.h"

#include "common/vector.h"
#include "common/matrix.h"

#include "objects/texture/texture.h"
#include "deko3d/CFont.h"

#if defined (_3DS)
    #define FONT_DEFAULT_SIZE 22.5f;
#elif defined (__SWITCH__)
    #define FONT_DEFAULT_SIZE 14.0f;
#endif

namespace love
{
    class Font : public Object
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

            #if defined (_3DS)
                enum SystemFontType
                {
                    TYPE_STANDARD  = CFG_REGION_USA,
                    TYPE_CHINESE   = CFG_REGION_CHN,
                    TYPE_TAIWANESE = CFG_REGION_TWN,
                    TYPE_KOREAN    = CFG_REGION_KOR,
                    TYPE_MAX_ENUM
                };
            #elif defined (__SWITCH__)
                enum SystemFontType
                {
                    TYPE_STANDARD               = PlSharedFontType_Standard,
                    TYPE_CHINESE_SIMPLIFIED     = PlSharedFontType_ChineseSimplified,
                    TYPE_CHINESE_TRADITIONAL    = PlSharedFontType_ChineseTraditional,
                    TYPE_CHINESE_SIMPLIFIED_EXT = PlSharedFontType_ExtChineseSimplified,
                    TYPE_KOREAN                 = PlSharedFontType_KO,
                    TYPE_NINTENDO_EXTENDED      = PlSharedFontType_NintendoExt,
                    TYPE_MAX_ENUM
                };
            #endif

            static love::Type type;

            static constexpr float DEFAULT_SIZE = FONT_DEFAULT_SIZE;

            Font(SystemFontType type, float size = DEFAULT_SIZE);

            Font(Data * data, float size = DEFAULT_SIZE);

            Font(float size = DEFAULT_SIZE);

            ~Font();

            void Print(Graphics * gfx, const std::vector<ColoredString> & text, float limit, const Colorf & color, AlignMode align, const Matrix4 & localTransform);

            void ClearBuffer();


            float GetWidth(const char * text);

            float GetHeight();

            float _GetGlyphWidth(u16 glyph);

            float GetSize();

            static bool GetConstant(const char * in, AlignMode & out);
            static bool GetConstant(AlignMode in, const char *& out);
            static std::vector<std::string> GetConstants(AlignMode);

            static bool GetConstant(const char * in, SystemFontType & out);
            static bool GetConstant(SystemFontType in, const char *& out);
            static std::vector<std::string> GetConstants(SystemFontType);

        private:
            CFont font;
            TextBuffer buffer;
            float size;
            TextHandle text;

            TextureHandle texture;

            float GetScale() { return this->size / 30.0f; }

            static StringMap<AlignMode, ALIGN_MAX_ENUM>::Entry alignModeEntries[];
            static StringMap<AlignMode, ALIGN_MAX_ENUM> alignModes;

            static StringMap<SystemFontType, SystemFontType::TYPE_MAX_ENUM>::Entry sharedFontEntries[];
            static StringMap<SystemFontType, SystemFontType::TYPE_MAX_ENUM> sharedFonts;
    };
}
