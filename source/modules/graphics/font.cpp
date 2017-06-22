#include "common/runtime.h"
#include "filesystem.h"
#include "image.h"
#include "file.h"
#include "quad.h"
#include "glyph.h"
#include "font.h"

using love::Font;
using love::Image;
using json = nlohmann::json;

char * Font::Init(const char * path)
{
	/*
		TODO:
		Check for path.json AND path.png
		Load path.json via jansson portlib
		Read and define values as needed
		Print text to the screen
		^ This is gonna be hell.. I already know it.. Well maybe not idk..
	*/
	std::string fontPath(path);
	std::string configPath(fontPath); 
	configPath.append(".json");
	
	std::string bitmapPath(fontPath); 
	bitmapPath.append(".png");

	if (love::Filesystem::Instance()->Exists(configPath.c_str()))
		this->configFile = new love::File();
	else
		return "Could not load font. Config does not exist.";

	if (love::Filesystem::Instance()->Exists(bitmapPath.c_str()))
		this->bitmap = new love::Image();
	else
		return "Could not load font. Bitmap does not exist.";

	this->configFile->InitPath(configPath.c_str());
	this->configFile->Open("r");
	
	std::string data(this->configFile->Read());

	this->configJson = json::parse(data);
	this->configFile->Close();

	this->bitmap->Init(bitmapPath.c_str());

	this->chars = this->configJson["chars"].size();
	printf("Glyph count: %d\n", this->chars);

	//printf("%s\n", this->configJson.dump().c_str());
	for (auto it = this->configJson["chars"].begin(); it != this->configJson["chars"].end(); it++)
		this->glyphs.push_back(love::Glyph(it.key().c_str(), it.value()));

	return nullptr;
}

love::Glyph Font::GetGlyph(char glyph)
{
	for (int i = 0; i < this->chars; i++)
	{
		if (glyph == this->glyphs[i].GetChar())
		{
			printf("!");
			return this->glyphs[i];
		}
		else
		{
			printf("%c vs %c!\n", glyph, this->glyphs[i].GetChar());
			return this->glyphs[i];
		}
	}

	return this->glyphs[0];
}

love::Image * Font::GetSheet()
{
	return this->bitmap;
}