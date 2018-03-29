#pragma once

class Glyph
{
	public:
		Glyph() {};
		Glyph(uint code, FT_GlyphSlot & slot, FT_Bitmap bitmap);

		int GetWidth();
		int GetBearingX();

		FT_Bitmap * GetBitmap();
		
		std::pair<int, int> GetCorner();
		std::pair<int, int> GetSize();
		std::pair<int, int> GetAdvance();

		int GetID();

	private:
		uint id;

		FT_Bitmap bitmap;

		int bearingX;
		int bearingY;

		int width;
		int height;
		
		int bitmapWidth;
		int bitmapHeight;

		int top;
		int left;

		int advanceX;
		int advanceY;
};