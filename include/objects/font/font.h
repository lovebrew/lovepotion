#pragma once

#include "common/exception.h"
#include "common/mmath.h"
#include "common/stringmap.h"

#include "common/vector.h"
#include "common/matrix.h"

#include "objects/texture/texture.h"

#if defined (_3DS)
    #define FONT_DEFAULT_SIZE 22.5f;
#elif defined (__SWITCH__)
    #define FONT_DEFAULT_SIZE 14.0f;

    #include "deko3d/CFont.h"
    #include "freetype/rasterizer.h"
#endif

namespace love
{
    class Font : public Object
    {
        public:
            typedef std::vector<uint32_t> Codepoints;

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

            struct IndexedColor
            {
                Colorf color;
                int index;
            };

            struct ColoredCodepoints
            {
                std::vector<uint32_t> codes;
                std::vector<IndexedColor> colors;
            };

            struct TextInfo
            {
                int width;
                int height;
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

                static void GetCodepointsFromString(const std::string & string, Codepoints & points);
                static void GetCodepointsFromString(const std::vector<ColoredString> & strings, ColoredCodepoints & codepoints);

                void GenerateVertices(const ColoredCodepoints & codepoints, const Colorf & constantcolor, std::vector<vertex::Vertex> & glyphVertices,
                                      float extra_spacing = 0.0f, Vector2 offset = {}, TextInfo * info = nullptr);

                void PrintV(Graphics * gfx, const Matrix4 & t, const std::vector<vertex::Vertex> & vertices);
            #endif

            enum Hinting
            {
                HINTING_NORMAL,
                HINTING_LIGHT,
                HINTING_MONO,
                HINTING_NONE,
                HINTING_MAX_ENUM
            };

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
            #if defined(__SWITCH__)
                CFont font;
                std::vector<vertex::Vertex> glyphVertices;
            #elif defined(_3DS)
                C2D_Font font;
            #endif

            TextBuffer buffer;
            float size;
            TextHandle text;

            TextureHandle texture;

            float GetLineHeight() { return 1.0f; }

            float GetScale() { return this->size / 30.0f; }

            static StringMap<AlignMode, ALIGN_MAX_ENUM>::Entry alignModeEntries[];
            static StringMap<AlignMode, ALIGN_MAX_ENUM> alignModes;

            static StringMap<SystemFontType, SystemFontType::TYPE_MAX_ENUM>::Entry sharedFontEntries[];
            static StringMap<SystemFontType, SystemFontType::TYPE_MAX_ENUM> sharedFonts;
    };
}
