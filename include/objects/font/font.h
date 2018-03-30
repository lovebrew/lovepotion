#pragma once

class Font
{
	public:
		Font(const char * path, int size);
		Font(int size = 16);
		~Font();

		SDL_Surface * GetSurface();
		bool HasGlyph(uint glyph);
		int GetWidth(uint glyph);
		int GetSize();
		TTF_Font * GetFont();

	private:
		TTF_Font * font;
		SDL_Surface * surface;
		float size;
};