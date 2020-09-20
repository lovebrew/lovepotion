#include "common/runtime.h"
#include "objects/font/font.h"

using namespace love;

#define FONT_NOT_FOUND_STRING "Could not find font %s (not converted to bcfnt?)"

Font::Font(Font::SystemFontType type, float size) : buffer(C2D_TextBufNew(4096)),
                                                    size(size)
{
    this->font = C2D_FontLoadSystem((CFG_Region)type);
}

Font::Font(Data * data, float size) : buffer(C2D_TextBufNew(4096)),
                                      size(size)
{
    this->font = C2D_FontLoadFromMem(data->GetData(), data->GetSize());
}

Font::Font(float size) : buffer(C2D_TextBufNew(4096)),
                         size(size)
{
    this->font = C2D_FontLoadSystem((CFG_Region)SystemFontType::TYPE_STANDARD);
}

Font::~Font()
{
    C2D_TextBufDelete(this->buffer);
    C2D_FontFree(this->font);
}

void Font::Print(const std::vector<Font::ColoredString> & strings, const DrawArgs & args,
                 float * limit, const Color & blend, Font::AlignMode align)
{
    C2D_Text text;
    u32 alignMode = C2D_AlignLeft;

    switch (align)
    {
        case Font::ALIGN_CENTER:
            alignMode = C2D_AlignCenter;
        case Font::ALIGN_RIGHT:
            alignMode = C2D_AlignRight;
        case Font::ALIGN_JUSTIFY:
            alignMode = C2D_AlignJustified | C2D_WordWrap;
        default:
            break;
    }

    std::string result = std::accumulate(strings.begin(), strings.end(), std::string{}, [](const std::string& s1, const ColoredString& piece) { return s1 + piece.string; });

    C2D_TextFontParse(&text, this->font, this->buffer, result.c_str());
    C2D_TextOptimize(&text);

    C2D_DrawText(&text, C2D_WithColor | alignMode, args.x, args.y, args.depth, this->GetScale() * args.scalarX, this->GetScale() * args.scalarY, C2D_Color32f(blend.r, blend.g, blend.b, blend.a), (limit != nullptr) ? *limit : 0.0f);
}

void Font::ClearBuffer()
{
    C2D_TextBufClear(this->buffer);
}

float Font::GetWidth(const char * text)
{
    float width = 0;
    C2D_Text measureText;
    C2D_TextBuf measureBuffer = C2D_TextBufNew(strlen(text));

    C2D_TextFontParse(&measureText, this->font, measureBuffer, text);

    if (strlen(text) != 0)
        C2D_TextGetDimensions(&measureText, this->GetScale(), this->GetScale(), &width, NULL);

    C2D_TextBufDelete(measureBuffer);

    return width;
}

float Font::_GetGlyphWidth(u16 glyph)
{
    fontGlyphPos_s out;
    C2D_FontCalcGlyphPos(this->font, &out, C2D_FontGlyphIndexFromCodePoint(this->font, glyph), 0, this->GetScale(), this->GetScale());

    return out.xAdvance;
}

float Font::GetHeight()
{
    return 30 * this->GetScale();
}

StringMap<Font::SystemFontType, Font::SystemFontType::TYPE_MAX_ENUM>::Entry Font::sharedFontEntries[] =
{
    { "standard",  TYPE_STANDARD  },
    { "chinese",   TYPE_CHINESE   },
    { "taiwanese", TYPE_TAIWANESE },
    { "korean",    TYPE_KOREAN    },
};

StringMap<Font::SystemFontType, Font::SystemFontType::TYPE_MAX_ENUM> Font::sharedFonts(Font::sharedFontEntries, sizeof(Font::sharedFontEntries));
