#include "common/runtime.h"

#include "modules/graphics.h"
#include "common/drawable.h"
#include "objects/font/font.h"

Font::Font(float size) : Drawable("Font")
{
    this->buffer = C2D_TextBufNew(4096);
    this->size = size;

    this->scale = size / 30.0f;
}

Font::Font() : Drawable("Font")
{
    this->buffer = C2D_TextBufNew(4096);
    this->size = 15;

    this->scale = 0.5f;
}

Font::~Font()
{
    C2D_TextBufDelete(this->buffer);
}

void Font::Print(const char * text, double x, double y, double rotation, double scalarX, double scalarY, Color color)
{
    C2D_TextBufClear(this->buffer);
    C2D_TextParse(&this->citroText, this->buffer, text);

    C2D_TextOptimize(&this->citroText);

    C2D_DrawText(&this->citroText, C2D_AtBaseline | C2D_WithColor, x, y, 0.5, this->scale, this->scale, Graphics::ConvertColor(color));
}

float Font::GetWidth(const char * text)
{
    float width = 0;
    C2D_Text measureText;
    C2D_TextBuf measureBuffer = C2D_TextBufNew(4096);

    C2D_TextParse(&measureText, measureBuffer, text);

    if (strlen(text) != 0)
        C2D_TextGetDimensions(&measureText, this->scale, this->scale, &width, NULL);

    C2D_TextBufDelete(measureBuffer);

    return width;
}

float Font::GetSize()
{
    return 30 * this->scale;
}