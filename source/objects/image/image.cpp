#include "common/runtime.h"

#include "objects/file/file.h"
#include "objects/image/image.h"
#include "modules/window.h"

#include "error_png.h"
#include "plus_png.h"

Image::Image(const char * path, bool memory)
{
	if (!memory)
	{
		File exists(path);

		exists.Open("rb");
		if (!exists.IsOpen())
			throw Exception("File does not exit: %s", path);
		exists.Close();

		this->texture = IMG_Load(path);
	}
	else
	{
		if (strncmp(path, "plus", 4) == 0)
			this->texture = IMG_Load_RW(SDL_RWFromMem((void *)plus_png, plus_png_size), 1);
		else if (strncmp(path, "error", 5) == 0)
			this->texture = IMG_Load_RW(SDL_RWFromMem((void *)error_png, error_png_size), 1);
	}

	this->width = this->texture->w;
	this->height = this->texture->h;
}

u16 Image::GetWidth()
{
	return this->width;
}

u16 Image::GetHeight()
{
	return this->height;
}

SDL_Surface * Image::GetImage()
{
	return this->texture;
}

Image::~Image() 
{
	SDL_FreeSurface(this->texture);
}
