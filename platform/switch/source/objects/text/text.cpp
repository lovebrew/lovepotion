#include "common/runtime.h"
#include "common/drawable.h"

#include "objects/font/font.h"
#include "objects/text/text.h"

Text::Text(Font * font, const char * text) : Drawable("Text")
{
    this->font = font;
    //this->surface = TTF_RenderText_Blended_Wrapped(font->GetFont(), text, SDL_Color{255, 255, 255, 255}, 1280);
}