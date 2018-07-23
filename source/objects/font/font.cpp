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

    this->surface = NULL;

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

    this->surface = NULL;
}

void Font::Print(const char * text, double x, double y, SDL_Color color)
{
    if (strlen(text) == 0)
        return;

    this->surface = TTF_RenderText_Blended_Wrapped(this->font, text, color, 1280);
    SDL_SetSurfaceAlphaMod(this->surface, color.a);

	float sx = Graphics::GetXScale();
	float sy = Graphics::GetYScale();
	
    SDL_Rect position = {x * sx, y * sy, (this->surface->w) * sx, (this->surface->h) * sy};
    SDL_BlitScaled(this->surface, NULL, Window::GetSurface(), &position);

    SDL_FreeSurface(this->surface);
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