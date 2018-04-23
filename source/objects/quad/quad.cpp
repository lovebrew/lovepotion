#include "runtime.h"
#include "quad.h"

Quad::Quad(float subTextureX, float subTextureY, float subTextureWidth, float subTextureHeight, float atlasWidth, float atlasHeight)
{
	this->x = subTextureX;
	this->y = subTextureY;

	this->width = subTextureWidth;
	this->height = subTextureHeight;
}

void Quad::SetViewport(int x, int y, int width, int height)
{
	this->x = x;
	this->y = y;

	this->width = width;
	this->height = height;
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