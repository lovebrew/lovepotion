#include "runtime.h"

#include "filesystem.h"
#include "image.h"

Image::Image(const char * path)
{
	string realPath = path;

	size_t found = realPath.find(".png");

	printf("%d\n", found);

	if (found != -1)
		realPath = realPath.replace(found, 4, ".t3x");

	this->sheet = C2D_SpriteSheetLoad(realPath.c_str());

	if (!this->sheet)
	{
		Console::ThrowError("Image does not exist: %s\n", realPath.c_str());
		return;
	}
	else
		this->image = C2D_SpriteSheetGetImage(this->sheet, 0);

	if (!this->image.subtex)
	{
		Console::ThrowError("No subtex for %s\n", realPath.c_str());
		return;
	}

	this->width = this->image.subtex->width;
	this->height = this->image.subtex->height;
}

int Image::GetWidth()
{
	return this->width;
}

int Image::GetHeight()
{
	return this->height;
}

C2D_Image Image::GetImage()
{
	return this->image;
}

Tex3DS_SubTexture * Image::GetSubTex()
{
	return &this->subtex;
}

Image::~Image()
{
	C2D_SpriteSheetFree(this->sheet);
}