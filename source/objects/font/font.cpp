#include "common/runtime.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "objects/font/font.h"
#include "modules/graphics.h"

#include "vera_ttf.h"

Font::Font(const char * path, int size)
{
	this->dpiScale = 1.0f;
	size = floorf(size * dpiScale + 0.5f);

	int error = FT_New_Face(Graphics::GetFreetypeLibrary(), path, 0, &this->face);

	if (error == FT_Err_Unknown_File_Format)
		throw Exception("Unsupported format: %s", path);
	else if (error)
		throw Exception("Invalid font file: %s", path);

	error = FT_Set_Pixel_Sizes(this->face, size, size);
}

Font::Font(int size)
{
	this->dpiScale = 1.0f;
	size = floorf(size * dpiScale + 0.5f);

	FT_New_Memory_Face(Graphics::GetFreetypeLibrary(), (const FT_Byte *)vera_ttf, vera_ttf_size, 0, &this->face);

	FT_Set_Pixel_Sizes(this->face, size, size);
}

Font::~Font()
{
	FT_Done_Face(this->face);
}

float Font::GetKerning(char currentChar, char previousChar, FT_Face face)
{
	FT_Vector kerning = {};
	FT_Get_Kerning(face, currentChar, previousChar, FT_KERNING_DEFAULT, &kerning);

	return (float)(kerning.x);
}

bool Font::HasGlyph(uint glyph)
{
	return FT_Get_Char_Index(face, glyph) != 0;
}

FT_Face Font::GetFace()
{
	return this->face;
}