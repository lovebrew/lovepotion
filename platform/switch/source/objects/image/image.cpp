#include "common/runtime.h"

#include "objects/file/file.h"

#include "common/drawable.h"
#include "objects/image/image.h"
#include "modules/display.h"

#include "dog_png.h"
#include "head_png.h"
#include "dogshadow_png.h"
#include "shadow_png.h"
#include "tail_png.h"
#include "cloud_png.h"
#include "tongue_png.h"

#include "warn_png.h"
#include "button_png.h"

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

    //this->surface = SDL_ConvertSurface(tempSurface, Display::GetSurface()->format, NULL);
    //SDL_SetSurfaceBlendMode(this->surface, SDL_BLENDMODE_BLEND);

    this->texture = SDL_CreateTextureFromSurface(Display::GetRenderer(), tempSurface);
    SDL_SetTextureBlendMode(this->texture, SDL_BLENDMODE_BLEND);

    this->width = tempSurface->w;
    this->height = tempSurface->h;

    this->viewport = {0, 0, this->width, this->height, this->width, this->height};

    SDL_FreeSurface(tempSurface);
}

Image::Image(u32 * buffer, size_t bufferSize)
{
    SDL_Surface * tempSurface = NULL;
    tempSurface = IMG_Load_RW(SDL_RWFromMem((void *)buffer, bufferSize), 1);

    this->width = tempSurface->w;
    this->height = tempSurface->h;

    this->viewport = {0, 0, this->width, this->height, this->width, this->height};

    this->texture = SDL_CreateTextureFromSurface(Display::GetRenderer(), tempSurface);

    SDL_FreeSurface(tempSurface);
}

SDL_Surface * Image::GetMemoryImage(const char * path)
{
    string name = path;
    name = name.substr(name.find(":") + 1);

    SDL_Surface * returnSurface = NULL;

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
    else if (name == "warn")
        returnSurface = IMG_Load_RW(SDL_RWFromMem((void *)warn_png, warn_png_size), 1);
    else
        returnSurface = IMG_Load_RW(SDL_RWFromMem((void *)button_png, button_png_size), 1);

    return returnSurface;
}
