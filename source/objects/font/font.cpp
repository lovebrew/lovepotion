#include "runtime.h"

#include "quad.h"
#include "file.h"

#include "glyph.h"
#include "font.h"

#include "vera12_t3x.h"
#include "vera12_json.h"

Font::Font(const char * path)
{
	if (path != NULL)
	{
		string realPath(path);

		size_t found = realPath.find(".ttf");
		realPath = realPath.replace(found, 4, "");
	
		string sheetPath = (realPath + ".t3x");
		string filePath = (realPath + ".json");

		this->sheet = C2D_SpriteSheetLoad(sheetPath.c_str());

		File fileConfig(filePath.c_str());
		fileConfig.Open("r");

		try
		{
			this->configJson = json::parse(fileConfig.Read());
		}
		catch (std::exception & e)
		{
			printf("%s", e.what());
			Console::ThrowError("Font config does not exist: %s.json", path);
		}

		fileConfig.Close();

		printf("Loaded font: %s\n", path);
	}
	else
	{
		this->sheet = C2D_SpriteSheetLoadFromMem(vera12_t3x, vera12_t3x_size);

		string data((char *)vera12_json, vera12_json_size);

		try
		{
			this->configJson = json::parse(data.c_str());
			printf("Loaded default font config!\n");
		}
		catch (std::exception & e)
		{
			printf("%s", e.what());
		}
	}

	if (!this->sheet)
	{
		if (path != NULL)
			Console::ThrowError("Font sheet does not exist: %s.t3x", path);
		else
			Console::ThrowError("Somehow failed to load default font");
	}

	this->image = C2D_SpriteSheetGetImage(this->sheet, 0);

	if (!this->image.subtex)
	{
		Console::ThrowError("No subtex for %s", path);
		return;
	}

	this->atlasWidth = this->image.subtex->width;
	this->atlasHeight = this->image.subtex->height;

	printf("Loading glyphs\n");

	for (auto it = this->configJson["chars"].begin(); it != this->configJson["chars"].end(); it++)
	{
		if (it.value().is_object())
		{
			this->glyphs[stoi(it.key())] = Glyph(it.value(), this->atlasWidth, this->atlasHeight);
		}
	}

	this->height = this->configJson["info"]["size"];

	printf("Glyphs loaded!\n");
}

C2D_Image Font::GetImage()
{
	return this->image;
}

bool Font::HasGlyph(int glyph)
{
	return this->glyphs.find(glyph) != this->glyphs.end();
}

Quad * Font::GetGlyph(int glyph)
{
	return this->glyphs[glyph].GetQuad();
}

int Font::GetGlyphData(int glyph, int index)
{
	if (this->HasGlyph(glyph))
	{
		switch (index)
		{
			case 0:
				return this->glyphs[glyph].GetXOffset();
			case 1:
				return this->glyphs[glyph].GetYOffset();
			default:
				return this->glyphs[glyph].GetXAdvance();
		}
	}
	return NULL;
}

float Font::GetWidth(int glyph)
{
	if (this->HasGlyph(glyph))
		return this->glyphs[glyph].GetXAdvance();

	return 0;
}

float Font::GetHeight()
{
	return this->height;
}