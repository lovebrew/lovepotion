#include "common/runtime.h"
#include "quad.h"

using love::Quad;

const char * Quad::Init(int x, int y, int width, int height)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;

	return nullptr;
}

int Quad::GetX()
{
	return this->x;
}

int Quad::GetY()
{
	return this->y;
}

int Quad::GetWidth()
{
	return this->width;
}

int Quad::GetHeight()
{
	return this->height;
}