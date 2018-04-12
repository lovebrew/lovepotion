#include "common/runtime.h"

#include "common/drawable.h"
#include "objects/canvas/canvas.h"
#include "modules/window.h"

Canvas::Canvas(int width, int height)
{
	this->texture = SDL_CreateTexture(Window::GetRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);

	this->width = width;
	this->height = height;
}

void Canvas::SetAsTarget()
{
	SDL_SetRenderTarget(Window::GetRenderer(), this->texture);
}