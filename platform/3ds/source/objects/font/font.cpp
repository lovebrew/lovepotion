#include "common/runtime.h"

#include "modules/graphics.h"
#include "common/drawable.h"
#include "objects/font/font.h"

Font::Font(const string & name, float size) : Drawable("Font")
{
    this->buffer = C2D_TextBufNew(4096);
    this->size = size;

    this->font = this->LoadFont(name);
    this->scale = size / 30.0f;
}

Font::Font() : Drawable("Font")
{
    this->buffer = C2D_TextBufNew(4096);
    this->size = 15;

    this->scale = 0.5f;
    this->font = NULL;
}

Font::~Font()
{
    C2D_TextBufDelete(this->buffer);
    C2D_FontFree(this->font);
}

C2D_Font Font::LoadFont(const string & name)
{
    //check for system fonts
    //JPN, USA, EUR, and AUS all use the same font.

    if (name == "standard")
        return C2D_FontLoadSystem(CFG_REGION_USA);
    else if (name == "korean")
        return C2D_FontLoadSystem(CFG_REGION_KOR);
    else if (name == "taiwanese")
        return C2D_FontLoadSystem(CFG_REGION_TWN);
    else if (name == "chinese")
        return C2D_FontLoadSystem(CFG_REGION_CHN);
    
    const char * filename = name.c_str();
    return C2D_FontLoad(filename);
}

void Font::Print(const char * text, double x, double y, double rotation, double scalarX, double scalarY, Color color)
{
    C2D_TextBufClear(this->buffer);
    C2D_TextFontParse(&this->citroText, this->font, this->buffer, text);

    C2D_TextOptimize(&this->citroText);

    C2D_DrawText(&this->citroText, C2D_WithColor, x, y, 0.5, this->scale, this->scale, Graphics::ConvertColor(color));
}

float Font::GetWidth(const char * text)
{
    float width = 0;
    C2D_Text measureText;
    C2D_TextBuf measureBuffer = C2D_TextBufNew(4096);

    C2D_TextFontParse(&measureText, this->font, measureBuffer, text);

    if (strlen(text) != 0)
        C2D_TextGetDimensions(&measureText, this->scale, this->scale, &width, NULL);

    C2D_TextBufDelete(measureBuffer);

    return width;
}

float Font::GetSize()
{
    return 30 * this->scale;
}
