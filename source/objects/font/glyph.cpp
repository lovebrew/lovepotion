#include "common/runtime.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "objects/font/glyph.h"

Glyph::Glyph(uint code, FT_GlyphSlot & slot, FT_Bitmap bitmap)
{
	this->id = code;

	this->advanceX = slot->advance.x >> 6;
	this->advanceY = slot->advance.y >> 6;

	this->bitmapWidth = bitmap.width;
	this->bitmapHeight = bitmap.rows;

	this->top = slot->bitmap_top;
	this->left = slot->bitmap_left;

	this->bitmap = bitmap;
}

std::pair<int, int> Glyph::GetCorner()
{
	return std::make_pair(this->top, this->left);
}

std::pair<int, int> Glyph::GetAdvance()
{
	return std::make_pair(this->advanceX, this->advanceY);
}

std::pair<int, int> Glyph::GetSize()
{
	return std::make_pair(this->width, this->height);
}

int Glyph::GetID()
{
	return this->id;
}

FT_Bitmap * Glyph::GetBitmap()
{
	return &this->bitmap;
}