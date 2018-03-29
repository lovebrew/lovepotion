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
		int GetGlyphData(FT_UInt glyph, FT_Face face, const std::string & field);
		int GetWidth(uint kerning, Glyph * glyph);
		int GetSize();

	private:
		FT_Face face;
		float dpiScale;
		std::map<FT_UInt, Glyph> glyphs;
		int size;
};