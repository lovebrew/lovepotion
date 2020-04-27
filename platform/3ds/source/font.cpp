#include "common/runtime.h"
#include "objects/font/font.h"

using namespace love;

#define FONT_NOT_FOUND_STRING "Could not find font %s (not converted to fnt?)"

Font::Font(const std::string & path, float size) : buffer(C2D_TextBufNew(4096)),
                                                   size(size)
{
    this->font = this->LoadFromPath(path);
}

Font::Font(float size) : buffer(C2D_TextBufNew(4096)),
                         size(size)
{
    this->font = this->LoadFromPath("standard");
}

Font::~Font()
{
    C2D_TextBufDelete(this->buffer);
    C2D_FontFree(this->font);
}

FontHandle Font::LoadFromPath(const std::string & path)
{
    if (path == "standard")
        return C2D_FontLoadSystem(CFG_REGION_USA);
    else if (path == "korean")
        return C2D_FontLoadSystem(CFG_REGION_KOR);
    else if (path == "taiwanese")
        return C2D_FontLoadSystem(CFG_REGION_TWN);
    else if (path == "chinese")
        return C2D_FontLoadSystem(CFG_REGION_CHN);

    size_t pos = path.find_last_of(".") + 1;
    std::string translation;

    if (pos != std::string::npos)
        translation = (path.substr(0, pos) + "fnt");

    if (std::filesystem::exists(translation))
        return C2D_FontLoad(translation.c_str());
    else
        throw love::Exception(FONT_NOT_FOUND_STRING, path.c_str());

    return NULL;
}

std::pair<float, float> Font::GenerateVertices(const std::string & line, const std::pair<float, float> & offset,
                                               const DrawArgs & args, const Color & blend, const Color & color)
{
    C2D_Text text;
    const char * str = line.c_str();

    C2D_TextFontParse(&text, this->font, this->buffer, str);
    C2D_TextOptimize(&text);

    float width;
    float height;

    C2D_TextGetDimensions(&text, this->GetScale(), this->GetScale(), &width, &height);

    Color res = Colors::ALPHA_BLEND_COLOR(color, blend);
    u32 currentColor = C2D_Color32f(res.r, res.g, res.b, res.a);

    C2D_DrawText(&text, C2D_WithColor, args.x + offset.first, args.y + offset.second, args.depth, this->GetScale() * args.scalarX, this->GetScale() * args.scalarY, currentColor);

    return std::pair(width, height);
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

bool Font::GetConstant(const char * in, AlignMode & out)
{
    return alignModes.Find(in, out);
}

bool Font::GetConstant(AlignMode in, const char  *& out)
{
    return alignModes.Find(in, out);
}

std::vector<std::string> Font::GetConstants(AlignMode)
{
    return alignModes.GetNames();
}

StringMap<Font::AlignMode, Font::ALIGN_MAX_ENUM>::Entry Font::alignModeEntries[] =
{
    { "left",    ALIGN_LEFT    },
    { "right",   ALIGN_RIGHT   },
    { "center",  ALIGN_CENTER  },
    { "justify", ALIGN_JUSTIFY },
};

StringMap<Font::AlignMode, Font::ALIGN_MAX_ENUM> Font::alignModes(Font::alignModeEntries, sizeof(Font::alignModeEntries));
