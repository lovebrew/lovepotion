#include "common/runtime.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "modules/filesystem.h"

#include "objects/font/glyph.h"
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

	error = FT_Set_Char_Size(this->face, 0, size << 6, 72, 72);

	FT_UInt index;
	FT_ULong character = FT_Get_First_Char(this->face, &index);

	while (index != 0)
	{
		FT_Load_Glyph(this->face, character, FT_LOAD_DEFAULT);
		
		this->glyphs[character] = new Glyph(index, this->face->glyph, this->face->glyph->bitmap);
		
		character = FT_Get_Next_Char(this->face, character, &index);
	}
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

int Font::GetKerning(int currentChar, int previousChar)
{
	FT_Vector kerning;
	FT_Get_Kerning(face, previousChar, currentChar, FT_KERNING_DEFAULT, &kerning);
	
	return (kerning.x >> 6);
}

int Font::GetWidth(uint kerning, Glyph * glyph)
{
	int width = 0;
	std::pair<int, int> size = glyph->GetCorner();

	if ((glyph->GetXAdvance() + kerning) < (size.first + kerning))
		width += size.first + kerning;
	else
		width += glyph->GetXAdvance() + kerning;

	return width;
}

FT_Face Font::GetFace()
{
	return this->face;
}

bool Font::HasGlyph(uint glyph)
{
	return this->glyphs.find(glyph) != this->glyphs.end();
}

Glyph * Font::GetGlyph(uint glyph)
{
	if (this->HasGlyph(glyph))
		return this->glyphs[glyph];
	else
		return nullptr; //spaaaaace
}