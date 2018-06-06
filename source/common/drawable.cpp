#include "common/runtime.h"
#include "common/drawable.h"
#include "modules/window.h"

Drawable::Drawable()
{
    this->texture = NULL;
    this->surface = NULL;
    this->changedSurface = NULL;

    this->horScale = 0.0;
}

void Drawable::Draw(SDL_Rect * quad, double x, double y, double rotation, double scalarX, double scalarY, SDL_Color color)
{

    double scaledWidth = this->width * scalarX;
    double scaledHeight = this->height * scalarY;

    SDL_Rect positionRectangle = {x, y, scaledWidth, scaledHeight};

    if (quad != NULL)
    {
        positionRectangle.w = quad->w * scalarX;
        positionRectangle.h = quad->h * scalarY;
    }
    
    if (this->surface != NULL)
    {
        /*if (this->horScale != scalarX)
        {
            this->changedSurface = rotozoomSurfaceXY(this->surface, rotation, scalarX, scalarY, 0);
            SDL_SetSurfaceBlendMode(this->changedSurface, SDL_BLENDMODE_BLEND);

            this->horScale = scalarX;
        }*/

        SDL_SetSurfaceColorMod(this->surface, color.r, color.g, color.b);
        SDL_SetSurfaceAlphaMod(this->surface, color.a);

        if (quad != NULL)
        {
            int origin = quad->x;
            if (scalarX < 0.0)
            {
                quad->x = (this->width - quad->w - origin);
                positionRectangle.x -= quad->w;
            }
        }

        SDL_BlitScaled(this->surface, quad, Window::GetSurface(), &positionRectangle);
    }
    else if (this->texture != NULL)
    {
        SDL_RendererFlip flip = SDL_FLIP_NONE;
        if (scalarX < 0.0)
            flip = SDL_FLIP_HORIZONTAL;

        SDL_SetTextureColorMod(this->texture, color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(this->texture, color.a);

        SDL_RenderCopyEx(Window::GetRenderer(), this->texture, quad, &positionRectangle, rotation, NULL, flip);
    }
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