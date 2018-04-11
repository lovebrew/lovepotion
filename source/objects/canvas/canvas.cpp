#include "common/runtime.h"

#include "objects/canvas/canvas.h"
#include "modules/window.h"

Canvas::Canvas(int width, int height)
{
	this->texture = SDL_CreateTexture(Window::GetRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);

	this->width = width;
	this->height = height;
}

SDL_Texture * Canvas::GetTexture()
{
	return this->texture;
}

int Canvas::GetWidth()
{
	return this->width;
}

int Canvas::GetHeight()
{
	return this->height;
}

Canvas::~Canvas()
{
	SDL_DestroyTexture(this->texture);
}