#include "common/runtime.h"

#include "modules/window/window.h"
#include "objects/font/font.h"

using namespace love;

#define WINDOW_MODULE() (Module::GetInstance<Window>(Module::M_WINDOW))

Font::Font(Data * data, float size) : size(size)
{
    // this->font = TTF_OpenFontRW(SDL_RWFromMem(data->GetData(), data->GetSize()), 1, size);
}

Font::Font(Font::SystemFontType type, float size) : size(size)
{
    PlFontData data;
    plGetSharedFontByType(&data, (PlSharedFontType)type);

    // this->font = TTF_OpenFontRW(SDL_RWFromMem(data.address, data.size), 1, size);
}

Font::Font(float size) : size(size)
{
    PlFontData data;
    plGetSharedFontByType(&data, (PlSharedFontType)SystemFontType::TYPE_STANDARD);

    // this->font = TTF_OpenFontRW(SDL_RWFromMem(data.address, data.size), 1, size);
}

Font::~Font()
{
    // TTF_CloseFont(this->font);
}

void Font::RenderLine(const std::string & line, love::Vector2 & offset, const DrawArgs & args,
                      const Colorf & blend, float wrap, Font::AlignMode align, bool isNewLine)
{}

void Font::Print(Graphics * gfx, const std::vector<ColoredString> & text,
                 float limit, const Colorf & color, AlignMode align,
                 const Matrix4 & localTransform)
{}

float Font::GetSize()
{
    // return TTF_FontHeight(this->font);
}

float Font::GetHeight()
{
    // int height = 0;
    // TTF_SizeUTF8(this->font, " ", nullptr, &height);

    // return height;
}

float Font::_GetGlyphWidth(u16 glyph)
{
    // int xAdvance = 0;

    // TTF_GlyphMetrics(this->font, glyph, nullptr, nullptr, nullptr, nullptr, &xAdvance);

    // return xAdvance;
}

float Font::GetWidth(const char * text)
{
    // int width = 0;

    // if (strlen(text) != 0)
    //     TTF_SizeUTF8(this->font, text, &width, nullptr);

    // return width;
}

void Font::ClearBuffer()
{}

StringMap<Font::SystemFontType, Font::SystemFontType::TYPE_MAX_ENUM>::Entry Font::sharedFontEntries[] =
{
    { "standard",                    TYPE_STANDARD               },
    { "chinese simplified",          TYPE_CHINESE_SIMPLIFIED     },
    { "chinese traditional",         TYPE_CHINESE_TRADITIONAL    },
    { "extended chinese simplified", TYPE_CHINESE_SIMPLIFIED_EXT },
    { "korean",                      TYPE_KOREAN                 },
    { "nintendo extended",           TYPE_NINTENDO_EXTENDED      }
};

StringMap<Font::SystemFontType, Font::SystemFontType::TYPE_MAX_ENUM> Font::sharedFonts(Font::sharedFontEntries, sizeof(Font::sharedFontEntries));
