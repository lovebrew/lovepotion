#pragma once

class Font
{
	public:
		Font() {};
		Font(const char * path);

		C2D_Image GetImage();
		float GetHeight();
		void LoadGlyphs();

		float GetWidth(int glyph);
		Quad * GetGlyph(int glyph);
		bool HasGlyph(int glyph);
		int GetGlyphData(int glyph, int index);

	private:
		C2D_SpriteSheet sheet;
		C2D_Image image;
		nlohmann::json configJson;

		int atlasWidth;
		int atlasHeight;

		int height;

		std::map<int, Glyph> glyphs;
};