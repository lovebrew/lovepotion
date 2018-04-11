#include "common/runtime.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "modules/filesystem.h"

#include "objects/font/font.h"
#include "modules/graphics.h"

#include "vera_ttf.h"

Font::Font(const char * path, int size)
{
	float dpiScale = 1.0f;
	this->size = floorf(size * dpiScale + 0.5f);

	this->font = TTF_OpenFont(path, this->size);
}

Font::Font(int size)
{
	float dpiScale = 1.0f;
	size = floorf(size * dpiScale + 0.5f);

	this->font = TTF_OpenFontRW(SDL_RWFromMem((void *)vera_ttf, vera_ttf_size), 0, size);
}

Font::~Font()
{
	TTF_CloseFont(this->font);
}

int Font::GetSize()
{
	return this->size;
}

int Font::GetWidth(uint glyph)
{
	/*int width = 0;
	std::pair<int, int> size = glyph->GetCorner();

	if ((glyph->GetXAdvance() + kerning) < (size.first + kerning))
		width += size.first + kerning;
	else
		width += glyph->GetXAdvance() + kerning;
	*/

	return 0;
}

TTF_Font * Font::GetFont()
{
	return this->font;
}

bool Font::HasGlyph(uint glyph)
{
	return TTF_GlyphIsProvided(this->font, glyph);
}