#pragma once

class Font
{
	public:
		Font(const char * path, int size);
		Font(int size = 16);
		~Font();

		FT_Face GetFace();
		float GetKerning(char currentChar, char previousChar, FT_Face face);
		bool HasGlyph(uint glyph);

	private:
		FT_Face face;
		float dpiScale;
};