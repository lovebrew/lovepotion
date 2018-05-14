#include "runtime.h"
#include "quad.h"

Quad::Quad(float subTextureX, float subTextureY, float subTextureWidth, float subTextureHeight, float atlasWidth, float atlasHeight)
{
	int citroWidth = this->NextPow2(atlasWidth);
	int citroHeight = this->NextPow2(atlasHeight);

	this->u0 = subTextureX/citroWidth;
	this->v0 = subTextureY/citroHeight;

	this->u1 = (subTextureX + subTextureWidth)/citroWidth;
	this->v1 = (subTextureY + subTextureHeight)/citroHeight;

	this->width = subTextureWidth;
	this->height = subTextureHeight;
}

void Quad::SetSubTexture(Tex3DS_SubTexture * subtexture)
{
	subtexture->top 	= 1.0f - this->v0;
	subtexture->left 	= this->u0;
	subtexture->right 	= this->u1;
	subtexture->bottom 	= 1.0f - this->v1;

	subtexture->width = this->width;
	subtexture->height = this->height;
}

int Quad::NextPow2(unsigned int x)
{
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x++;
	return std::min(std::max(x, 8U), 1024U); // clamp size to keep gpu from locking
}