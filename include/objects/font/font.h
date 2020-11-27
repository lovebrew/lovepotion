#pragma once

#include "common/exception.h"
#include "common/mmath.h"
#include "common/stringmap.h"

#include "common/vector.h"
#include "common/matrix.h"

#include "objects/image/image.h"

#if defined (_3DS)
    #define FONT_DEFAULT_SIZE 22.5f;
#elif defined (__SWITCH__)
    #define FONT_DEFAULT_SIZE 14.0f;

    #include "deko3d/vertex.h"
    #include "freetype/rasterizer.h"
    #include "freetype/types/truetyperasterizer.h"
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

                struct DrawCommand
                {
                    int startVertex;
                    int vertexCount;

                    love::Texture * texture;
                };

                static void GetCodepointsFromString(const std::string & string, Codepoints & points);
                static void GetCodepointsFromString(const std::vector<ColoredString> & strings, ColoredCodepoints & codepoints);

                std::vector<DrawCommand> GenerateVertices(const ColoredCodepoints & codepoints, const Colorf & constantcolor,
                                                          std::vector<vertex::GlyphVertex> & glyphVertices, float extra_spacing = 0.0f,
                                                          Vector2 offset = {}, TextInfo * info = nullptr);

                std::vector<DrawCommand> GenerateVerticesFormatted(const ColoredCodepoints & text, const Colorf & constantColor, float wrap,
                                                                   AlignMode align, std::vector<vertex::GlyphVertex> & vertices, TextInfo * info = nullptr);

                void PrintV(Graphics * gfx, const Matrix4 & t, const std::vector<DrawCommand> & drawcommands, const std::vector<vertex::GlyphVertex> & vertices);

                void GetWrap(const std::vector<ColoredString> & text, float wraplimit, std::vector<std::string> & lines, std::vector<int> * lineWidths = nullptr);

                void GetWrap(const ColoredCodepoints & codepoints, float wraplimit, std::vector<ColoredCodepoints> & lines, std::vector<int> * lineWidths = nullptr);
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

            #if defined (__SWITCH__)
                struct Glyph
                {
                    love::Texture * texture;
                    int spacing;
                    vertex::GlyphVertex vertices[4];
                };

                Font(Rasterizer * r, const Texture::Filter & filter);

                const Font::Glyph & FindGlyph(uint32_t glyph);

                const Font::Glyph & AddGlyph(uint32_t glyph);

                love::GlyphData * GetRasterizerGlyphData(uint32_t glyph);

                float GetKerning(uint32_t leftGlyph, uint32_t rightGlyph);

                float GetAscent() const;

                float GetBaseline() const;

                float GetDescent() const;

                static const int SPACES_PER_TAB = 4;

                int GetWidth(char character);
            #endif

            Font(float size = DEFAULT_SIZE);

            ~Font();

            void Print(Graphics * gfx, const std::vector<ColoredString> & text, const Matrix4 & localTransform, const Colorf & color);
            void Printf(Graphics * gfx, const std::vector<ColoredString> & text, float wrap, AlignMode align, const Matrix4 & localTransform, const Colorf & color);

            void ClearBuffer();


            float GetWidth(const char * text);

            float GetHeight() const;

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
                struct TextureSize
                {
                    int width;
                    int height;
                };

                TextureSize GetNextTextureSize() const;

                std::vector<love::StrongReference<love::Rasterizer>> rasterizers;

                int height;
                int lineHeight;

                int textureWidth;
                int textureHeight;

                bool useSpacesAsTab;

                uint32_t textureCacheID;

                float dpiScale;

                int textureX;
                int textureY;
                int rowHeight;

                static const int TEXTURE_PADDING = 4;

                std::unordered_map<uint32_t, Font::Glyph> glyphs;

                std::vector<love::StrongReference<love::Image>> images;

                std::unordered_map<uint64_t, float> kerning;

                void CreateTexture();
            #elif defined(_3DS)
                C2D_Font font;
            #endif

            TextBuffer buffer;
            float size;
            TextHandle text;

            TextureHandle texture;

            float GetLineHeight() const;

            float GetScale() { return this->size / 30.0f; }

            static StringMap<AlignMode, ALIGN_MAX_ENUM>::Entry alignModeEntries[];
            static StringMap<AlignMode, ALIGN_MAX_ENUM> alignModes;

            static StringMap<SystemFontType, SystemFontType::TYPE_MAX_ENUM>::Entry sharedFontEntries[];
            static StringMap<SystemFontType, SystemFontType::TYPE_MAX_ENUM> sharedFonts;
    };
}
