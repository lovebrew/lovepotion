#include "common/runtime.h"

#include "modules/filesystem.h"

#include "objects/font/font.h"
#include "modules/graphics.h"
#include "modules/window.h"

#include "vera_ttf.h"

Font::Font(const char * path, int size)
{
	float dpiScale = 1.0f;
	this->size = floorf(size * dpiScale + 0.5f);

	this->font = TTF_OpenFont(path, this->size);

	this->surface = NULL;
}

Font::Font(int size)
{
	float dpiScale = 1.0f;
	size = floorf(size * dpiScale + 0.5f);

	this->font = TTF_OpenFontRW(SDL_RWFromMem((void *)vera_ttf, vera_ttf_size), 1, size);

	this->surface = NULL;
}

void Font::Print(const char * text, double x, double y, SDL_Color color)
{
	this->surface = TTF_RenderText_Blended_Wrapped(this->font, text, color, 1280);

	SDL_Rect position = {x, y, this->surface->w, this->surface->h};
	SDL_BlitSurface(this->surface, NULL, Window::GetSurface(), &position);

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
	int width;
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