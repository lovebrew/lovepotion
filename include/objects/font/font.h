#pragma once

class Font
{
	public:
		Font(const char * path, int size);
		Font(int size = 16);
		~Font();

		FT_Face GetFace();
		int GetKerning(int currentChar, int previousChar);
		bool HasGlyph(uint glyph);
		Glyph * GetGlyph(uint code);
		int GetWidth(uint kerning, Glyph * glyph);

	private:
		FT_Face face;
		float dpiScale;
		std::map<int, Glyph *> glyphs;
};