#pragma once

#include "common/exception.h"
#include "common/mmath.h"
#include "common/stringmap.h"

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
                Color color;
            };

            enum AlignMode
            {
                ALIGN_LEFT,
                ALIGN_CENTER,
                ALIGN_RIGHT,
                ALIGN_JUSTIFY,
                ALIGN_MAX_ENUM
            };

            static love::Type type;

            static constexpr float DEFAULT_SIZE = FONT_DEFAULT_SIZE;

            Font(const std::string & path, float size = DEFAULT_SIZE);
            Font(float size = DEFAULT_SIZE);
            ~Font();

            void Print(const std::vector<ColoredString> & text, const DrawArgs & args, float * limit, const Color & color, AlignMode align);
            void ClearBuffer();


            float GetWidth(const char * text);
            float GetHeight();

            float _GetGlyphWidth(u16 glyph);

            float GetSize();

            static bool GetConstant(const char * in, AlignMode & out);
            static bool GetConstant(AlignMode in, const char *& out);
            static std::vector<std::string> GetConstants(AlignMode);

        private:
            FontHandle font;
            TextBuffer buffer;
            float size;
            TextHandle text;

            TextureHandle texture;

            std::pair<float, float> GenerateVertices(const std::string & line, const std::pair<float, float> & offset, const DrawArgs & args, const Color & blend, const Color & color);

            FontHandle LoadFromPath(const std::string & path);
            float GetScale() { return this->size / 30.0f; }

            static StringMap<AlignMode, ALIGN_MAX_ENUM>::Entry alignModeEntries[];
            static StringMap<AlignMode, ALIGN_MAX_ENUM> alignModes;
    };
}
