#include "common/runtime.h"
#include "common/drawable.h"
#include "modules/window.h"

Drawable::Drawable(char * type) : Object(type)
{
    this->texture = NULL;
    this->surface = NULL;

    this->horScale = 0.0;
}

void Drawable::Draw(const Viewport & view, double x, double y, double rotation, double scalarX, double scalarY, SDL_Color color)
{
    SDL_Rect quad = {view.x, view.y, view.subWidth * scalarX, view.subHeight * scalarY};
    SDL_Rect position = {x, y, view.subWidth * scalarX, view.subHeight * scalarY};

    //SDL_Surface * surface = this->Flip(quad, position, rotation, scalarX, scalarY);

    if (this->surface != NULL)
    {
        SDL_SetSurfaceColorMod(this->surface, color.r, color.g, color.b);
        SDL_SetSurfaceAlphaMod(this->surface, color.a);

        SDL_BlitScaled(this->surface, &quad, Window::GetSurface(), &position);

        //SDL_FreeSurface(surface);
    }
    else if (this->texture != NULL)
    {
        SDL_RendererFlip flip = SDL_FLIP_NONE;
        if (scalarX < 0.0)
            flip = SDL_FLIP_HORIZONTAL;

        SDL_SetTextureColorMod(this->texture, color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(this->texture, color.a);

        SDL_RenderCopyEx(Window::GetRenderer(), this->texture, &quad, &position, rotation, NULL, flip);
    }
}

Viewport Drawable::GetViewport()
{
    return this->viewport;
}

SDL_Surface * Drawable::Flip(SDL_Rect & quad, SDL_Rect & position, double rotation, double x, double y)
{
    SDL_Surface * returnSurface = nullptr;

    if (this->horScale != x)
    {
        returnSurface = rotozoomSurfaceXY(this->surface, rotation, x, y, 0);
        SDL_SetSurfaceBlendMode(returnSurface, SDL_BLENDMODE_BLEND);

        if (x > 0.0)
            return returnSurface;
        
        int origin = quad.x;
        quad.x = (this->width - quad.w - origin);
        position.x -= quad.w;
    }

    return returnSurface;
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