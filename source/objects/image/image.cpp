#include "common/runtime.h"

#include "objects/file/file.h"

#include "common/drawable.h"
#include "objects/image/image.h"
#include "modules/window.h"

#include "error_png.h"
#include "plus_png.h"

Image::Image(const char * path, bool memory) : Drawable()
{
	SDL_Surface * tempSurface = NULL;

	if (!memory)
	{
		tempSurface = IMG_Load(path);
		if (!tempSurface)
			printf("Failed to load %s", path);
	}
	else
	{
		if (strncmp(path, "plus", 4) == 0)
			tempSurface = IMG_Load_RW(SDL_RWFromMem((void *)plus_png, plus_png_size), 1);
		else if (strncmp(path, "error", 5) == 0)
			tempSurface = IMG_Load_RW(SDL_RWFromMem((void *)error_png, error_png_size), 1);
	}

	this->surface = SDL_ConvertSurface(tempSurface, Window::GetSurface()->format, NULL);
	SDL_SetSurfaceBlendMode(this->surface, SDL_BLENDMODE_BLEND);

	this->width = this->surface->w;
	this->height = this->surface->h;

	SDL_FreeSurface(tempSurface);

	//this->texture = SDL_CreateTextureFromSurface(Window::GetRenderer(), surface);

}