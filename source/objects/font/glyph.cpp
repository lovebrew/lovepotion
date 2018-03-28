#include "common/runtime.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "objects/font/glyph.h"

Glyph::Glyph(uint code, FT_GlyphSlot slot, FT_Bitmap bitmap)
{
	this->id = code;

	this->advanceX = slot->advance.x / 64;
	this->advanceY = slot->advance.y / 64;

	this->bitmapWidth = bitmap.width;
	this->bitmapHeight = bitmap.rows;

	this->top = slot->bitmap_top;
	this->left = slot->bitmap_left;

	this->bearingX = slot->metrics.horiBearingX / 64;

	this->width = slot->metrics.width / 64;
	this->height = slot->metrics.height / 64;

	this->bitmap = bitmap;
}

std::pair<int, int> Glyph::GetCorner()
{
	return std::make_pair(this->top, this->left);
}

int Glyph::GetXAdvance()
{
	return this->advanceX;
}

std::pair<int, int> Glyph::GetSize()
{
	return std::make_pair(this->width, this->height);
}

int Glyph::GetID()
{
	return this->id;
}

int Glyph::GetBearingX()
{
	return this->bearingX;
}

FT_Bitmap * Glyph::GetBitmap()
{
	return &this->bitmap;
}