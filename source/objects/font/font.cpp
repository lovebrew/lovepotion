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
	this->size = floorf(size * dpiScale + 0.5f);

	int error = FT_New_Face(Graphics::GetFreetypeLibrary(), path, 0, &this->face);

	if (error == FT_Err_Unknown_File_Format)
		throw Exception("Unsupported format: %s", path);
	else if (error)
		throw Exception("Invalid font file: %s", path);

	error = FT_Set_Char_Size(this->face, 0, this->size << 6, 72, 72);
}

Font::Font(int size)
{
	this->dpiScale = 1.0f;
	size = floorf(size * dpiScale + 0.5f);

	FT_New_Memory_Face(Graphics::GetFreetypeLibrary(), (const FT_Byte *)vera_ttf, vera_ttf_size, 0, &this->face);

	FT_Set_Char_Size(this->face, 0, this->size << 6, 72, 72);
}

Font::~Font()
{
	FT_Done_Face(this->face);
}

int Font::GetSize()
{
	return this->size;
}

int Font::GetKerning(int currentChar, int previousChar)
{
	FT_Vector kerning;
	FT_Get_Kerning(face, previousChar, currentChar, FT_KERNING_DEFAULT, &kerning);
	
	return (kerning.x >> 6);
}

int Font::GetWidth(uint kerning, Glyph * glyph)
{
	/*int width = 0;
	std::pair<int, int> size = glyph->GetCorner();

	if ((glyph->GetXAdvance() + kerning) < (size.first + kerning))
		width += size.first + kerning;
	else
		width += glyph->GetXAdvance() + kerning;
	*/

	return 0;
}

FT_Face Font::GetFace()
{
	return this->face;
}

bool Font::HasGlyph(FT_UInt glyph)
{
	return this->glyphs.find(glyph) != this->glyphs.end();
}

int Font::GetGlyphData(FT_UInt glyph, FT_Face face, const string & field)
{
	if (!this->HasGlyph(glyph))
		this->glyphs[glyph] = Glyph(glyph, face->glyph, face->glyph->bitmap);

	if (field == "cornerX")
		return this->glyphs[glyph].GetCorner().first;
	else if (field == "cornerY")
		return this->glyphs[glyph].GetCorner().second;
}