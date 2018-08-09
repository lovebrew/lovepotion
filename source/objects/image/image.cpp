#include "common/runtime.h"

#include "objects/file/file.h"

#include "common/drawable.h"
#include "objects/image/image.h"
#include "modules/window.h"

#include "error_png.h"
#include "plus_png.h"

#include "dog_png.h"
#include "head_png.h"
#include "dogshadow_png.h"
#include "shadow_png.h"
#include "tail_png.h"
#include "cloud_png.h"
#include "tongue_png.h"

Image::Image(const char * path, bool memory) : Drawable("Image")
{
    SDL_Surface * tempSurface = NULL;

    if (!memory)
    {
        tempSurface = IMG_Load(path);
        if (!tempSurface)
            printf("Failed to load %s", path);
    }
    else
        tempSurface = this->GetMemoryImage(path);

    this->surface = SDL_ConvertSurface(tempSurface, Window::GetSurface()->format, NULL);
    SDL_SetSurfaceBlendMode(this->surface, SDL_BLENDMODE_BLEND);

    this->width = this->surface->w;
    this->height = this->surface->h;

    SDL_FreeSurface(tempSurface);
}

SDL_Surface * Image::GetMemoryImage(const char * path)
{
    string name = path;
    name = name.substr(name.find(":") + 1);

    SDL_Surface * returnSurface;

    if (name == "dog")
        returnSurface = IMG_Load_RW(SDL_RWFromMem((void *)dog_png, dog_png_size), 1);
    else if (name == "head")
        returnSurface = IMG_Load_RW(SDL_RWFromMem((void *)head_png, head_png_size), 1);
    else if (name == "dogshadow")
        returnSurface = IMG_Load_RW(SDL_RWFromMem((void *)dogshadow_png, dogshadow_png_size), 1);
    else if (name == "shadow")
        returnSurface = IMG_Load_RW(SDL_RWFromMem((void *)shadow_png, shadow_png_size), 1);
    else if (name == "tail")
        returnSurface = IMG_Load_RW(SDL_RWFromMem((void *)tail_png, tail_png_size), 1);
    else if (name == "tongue")
        returnSurface = IMG_Load_RW(SDL_RWFromMem((void *)tongue_png, tongue_png_size), 1);
    else if (name == "cloud")
        returnSurface = IMG_Load_RW(SDL_RWFromMem((void *)cloud_png, cloud_png_size), 1);
    else if (name == "error")
        returnSurface = IMG_Load_RW(SDL_RWFromMem((void *)error_png, error_png_size), 1);
    else
        returnSurface = IMG_Load_RW(SDL_RWFromMem((void *)plus_png, plus_png_size), 1);

    return returnSurface;
}