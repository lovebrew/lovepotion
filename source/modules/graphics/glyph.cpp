#include "common/runtime.h"
#include "quad.h"
#include "glyph.h"

using nlohmann::json;
using love::Glyph;

Glyph::Glyph(const char * glyph, json config)
{
	this->glyph = (char)std::stoi(glyph);

	int x = config["x"].get<int>();
	int y = config["y"].get<int>();
	int charWidth = config["width"].get<int>();
	int charHeight = config["height"].get<int>();

	this->quad = new love::Quad();
	this->quad->Init(x, y, charWidth, charHeight);

	this->width = config["xadvance"].get<int>();
}

love::Quad * Glyph::GetQuad()
{
	return this->quad;
}

int Glyph::GetOffsetX()
{
	return this->width;
}

char Glyph::GetChar()
{
	return this->glyph;
}