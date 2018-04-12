#include "common/runtime.h"
#include "common/drawable.h"
#include "modules/window.h"

void Drawable::Draw(SDL_Rect * quad, double x, double y, double rotation, double scalarX, double scalarY)
{
	SDL_Rect positionRectangle = {x, y, this->width, this->height};
	if (quad != NULL)
	{
		positionRectangle.w = quad->w;
		positionRectangle.h = quad->h;
	}
		
	SDL_RenderCopyEx(Window::GetRenderer(), this->texture, quad, &positionRectangle, rotation, NULL, SDL_FLIP_NONE);
}

int Drawable::GetWidth()
{
	return this->width;
}

int Drawable::GetHeight()
{
	return this->height;
}

Drawable::~Drawable()
{
	SDL_DestroyTexture(this->texture);
}