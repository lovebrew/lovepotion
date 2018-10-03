#include "common/runtime.h"
#include "common/drawable.h"
#include "modules/window.h"

Drawable::Drawable(char * type) : Object(type)
{
    this->texture = NULL;
    this->surface = NULL;

    this->flip = SDL_FLIP_NONE;
}

void Drawable::Draw(const Viewport & view, double x, double y, double rotation, double scalarX, double scalarY, SDL_Color color)
{
    SDL_Rect quad = {view.x, view.y, view.subWidth * abs(scalarX), view.subHeight * abs(scalarY)};
    SDL_Rect position = {x, y, view.subWidth * abs(scalarX), view.subHeight * abs(scalarY)};

    if (this->texture != NULL)
    {
        if (scalarX < 0.0)
        {
            if (this->flip != SDL_FLIP_HORIZONTAL)
                this->flip = SDL_FLIP_HORIZONTAL;
        }
        else if (scalarY < 0.0)
        {
            if (this->flip != SDL_FLIP_VERTICAL)
                this->flip = SDL_FLIP_VERTICAL;
        }
        else if (scalarX < 0.0 && scalarY < 0.0)
        {
            if (this->flip != (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL))
                this->flip = (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
        }
        else
            this->flip = SDL_FLIP_NONE;

        SDL_SetTextureColorMod(this->texture, color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(this->texture, color.a);

        SDL_RenderCopyEx(Window::GetRenderer(), this->texture, &quad, &position, rotation, NULL, this->flip);
    }
}

Viewport Drawable::GetViewport()
{
    return this->viewport;
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