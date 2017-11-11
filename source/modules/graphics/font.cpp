#include "common/runtime.h"
#include "filesystem.h"

#include "imagedata.h"

#include "image.h"
#include "file.h"
#include "quad.h"
#include "glyph.h"
#include "font.h"

#include "vera12_json.h"
#include "vera12_png.h"

using love::Font;
using json = nlohmann::json;

const char * Font::DefaultInit()
{
	std::string data((char *)vera12_json, vera12_json_size);

	this->configJson = json::parse(data);

	this->bitmap = new love::Image();
	this->bitmap->BasicInit(true);
	this->bitmap->DecodeMemory(vera12_png, vera12_png_size);

	this->chars = this->configJson["chars"].size();

	this->AddChars();

	return nullptr;
}

const char * Font::Init(const char * path)
{
	std::string fontPath(path);
	std::string configPath(fontPath); 
	configPath.append(".json");
	
	std::string bitmapPath(fontPath); 
	bitmapPath.append(".png");

	if (love::Filesystem::Instance()->Exists(configPath.c_str()))
		this->configFile = new love::File();
	else
		return path;

	this->bitmap = new love::Image();

	this->configFile->InitPath(configPath.c_str());
	this->configFile->Open("r");
	
	std::string data(this->configFile->Read());

	this->configJson = json::parse(data);
	this->configFile->Close();

	const char * bitmapError = this->bitmap->Init(bitmapPath.c_str(), false, true);
	if (bitmapError)
		return bitmapError;

	this->chars = this->configJson["chars"].size();
	
	this->AddChars();
	
	return nullptr;
}

void Font::AddChars()
{
	this->glyphs = new love::Glyph*[this->chars];
	int i = 0;

	for (auto it = this->configJson["chars"].begin(); it != this->configJson["chars"].end(); it++)
	{
		if (it.value().is_object())
		{
			this->glyphs[i] = new love::Glyph(it.value());
			i++;
		}
	}
}

love::Glyph * Font::GetGlyph(char glyph)
{	
	love::Glyph * ret = nullptr;
	for (int i = 0; i < this->chars; i++)
	{
		if ((int)glyph == this->glyphs[i]->GetChar())
		{
			ret = this->glyphs[i];
			break;
		}
	}

	return ret;
}

love::Image * Font::GetSheet()
{
	return this->bitmap;
}

int Font::GetWidth(char glyph)
{
	love::Glyph * charGlyph = this->GetGlyph(glyph);
	int width = 0;

	if (charGlyph != nullptr)
		width += charGlyph->GetXAdvance();

	return width;
}

int Font::GetHeight()
{
	return this->configJson["info"]["size"];
}

void Font::Collect()
{
	if (this->bitmap != nullptr)
		delete this->bitmap;

	if (this->configFile != nullptr)
		delete this->configFile;

	if (this->glyphs)
	{
		for (int i = 0; i < this->chars; i++)
			delete[] this->glyphs[i];
		
		delete[] this->glyphs;
	}
}