#include "common/runtime.h"

#include "objects/file/file.h"
#include "objects/image/image.h"
#include "modules/window.h"

Image::Image(const char * path)
{
	File exists(path);

	exists.Open("rb");
	if (!exists.IsOpen())
		throw Exception("File does not exit: %s", path);
	exists.Close();

	SDL_Surface * surface = IMG_Load(path);

	this->texture = SDL_CreateTextureFromSurface(Window::GetRenderer(), surface);

	this->width = surface->w;
	this->height = surface->h;

	SDL_FreeSurface(surface);
}

u16 Image::GetWidth()
{
	return this->width;
}

u16 Image::GetHeight()
{
	return this->height;
}

SDL_Texture * Image::GetImage()
{
	return this->texture;
}

Image::~Image() {}
