#include "runtime.h"

#include "quad.h"
#include "glyph.h"

Glyph::Glyph(json values, int sheetWidth, int sheetHeight)
{
	this->quad = Quad(values["x"], values["y"], values["width"], values["height"], sheetWidth, sheetHeight);

	this->xoffset = values["xoffset"];
	this->yoffset = values["yoffset"];

	this->xadvance = values["xadvance"];
}

Quad * Glyph::GetQuad()
{
	return &this->quad;
}

int Glyph::GetXOffset()
{
	return this->xoffset;
}

int Glyph::GetYOffset()
{
	return this->yoffset;
}

int Glyph::GetXAdvance()
{
	return this->xadvance;
}