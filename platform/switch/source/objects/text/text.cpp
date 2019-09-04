#include "common/runtime.h"

#include "modules/window.h"
#include "modules/display.h"
#include "common/drawable.h"

#include "objects/font/font.h"
#include "objects/text/text.h"

Text::Text(Font * font, const char * text) : Drawable("Text")
{
    this->SetFont(font);
    this->Set(text, 1280, "left");
}

void Text::Clear()
{
    SDL_DestroyTexture(this->texture);
    this->Set("", 1280, "left");
}

Font * Text::GetFont()
{
    return this->font;
}

void Text::SetFont(Font * font)
{
    this->font = font;
}

void Text::Set(const char * text, int limit, const string & align)
{
    if (strlen(text) == 0)
        return;

    SDL_Surface * tempSurface = TTF_RenderUTF8_Blended_Wrapped(this->font->GetFont(), text, SDL_Color{255, 255, 255, 255}, limit);

    this->texture = SDL_CreateTextureFromSurface(Display::GetRenderer(), tempSurface);
    SDL_SetTextureBlendMode(this->texture, SDL_BLENDMODE_BLEND);

    SDL_FreeSurface(tempSurface);

    int width, height = 0;
    SDL_QueryTexture(this->texture, NULL, NULL, &width, &height);

    this->viewport = {0, 0, width, height, width, height};

    TTF_SizeUTF8(this->font->GetFont(), text, &this->width, NULL);
    this->height = TTF_FontHeight(this->font->GetFont());
}
