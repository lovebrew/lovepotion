#include "common/runtime.h"

#include "modules/filesystem.h"

#include "objects/font/font.h"
#include "modules/graphics.h"
#include "modules/window.h"

Font::Font(const char * path, int size) : Object("Font")
{
    float dpiScale = 1.0f;
    this->size = floorf(size * dpiScale + 0.5f);

    this->font = TTF_OpenFont(path, this->size);

    this->texture = NULL;

    if (!this->font)
        printf("Font error: %s", TTF_GetError());
}

Font::Font(int size)
{
    u64 languageCode = 0;
    PlFontData fontData;
    PlFontData fonts[PlSharedFontType_Total];
    size_t total_fonts = 0;

    //setGetSystemLanguage(&languageCode);
    plGetSharedFont(languageCode, fonts, PlSharedFontType_Total, &total_fonts);
    plGetSharedFontByType(&fontData, PlSharedFontType_Standard);

    float dpiScale = 1.0f;
    size = floorf(size * dpiScale + 0.5f);

    this->font = TTF_OpenFontRW(SDL_RWFromMem((void *)fontData.address, fontData.size), 1, size);

    this->texture = NULL;
}

void Font::Print(const char * text, double x, double y, float limit, const string & align, SDL_Color color)
{
    if (strlen(text) == 0)
        return;

    SDL_Surface * tempSurface = TTF_RenderText_Blended_Wrapped(this->font, text, color, limit);
    this->texture = SDL_CreateTextureFromSurface(Window::GetRenderer(), tempSurface);
    SDL_FreeSurface(tempSurface);

    SDL_SetTextureColorMod(this->texture, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(this->texture, color.a);

    if (align == "center")
        x += (limit / 2);
    else if (align == "right")
        x += limit;

    int width, height = 0;
    SDL_QueryTexture(this->texture, NULL, NULL, &width, &height);
    SDL_Rect position = {x, y, width, height};

    SDL_RenderCopyEx(Window::GetRenderer(), this->texture, NULL, &position, 0, NULL, SDL_FLIP_NONE);

    SDL_DestroyTexture(this->texture);
}

Font::~Font()
{
    TTF_CloseFont(this->font);
}

int Font::GetSize()
{
    return this->size;
}

int Font::GetWidth(const char * text)
{
    int width = 0;

    if (strlen(text) != 0)
        TTF_SizeUTF8(this->font, text, &width, NULL);

    return width;
}

TTF_Font * Font::GetFont()
{
    return this->font;
}

bool Font::HasGlyph(uint glyph)
{
    return TTF_GlyphIsProvided(this->font, glyph);
}