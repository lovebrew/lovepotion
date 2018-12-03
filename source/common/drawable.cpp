#include "common/runtime.h"
#include "common/drawable.h"
#include "modules/window.h"

Drawable::Drawable(const string & type) : Object(type)
{
    this->texture = NULL;

    this->flip = SDL_FLIP_NONE;
}

void Drawable::Draw(SDL_Texture * texture, Viewport view, double x, double y, double rotation, double scalarX, double scalarY, SDL_Color color)
{
    if (!texture)
        texture = this->texture;

    SDL_Rect quad = 
    {
        view.x, 
        view.y, 
        view.subWidth, 
        view.subHeight
    };

    this->Flip(scalarX, scalarY);

    if (scalarX < 0)
        x -= (quad.w * abs(scalarX));
    else if (scalarY < 0)
        y -= (quad.h * abs(scalarY));
    else if (scalarX < 0 and scalarY < 0)
    {
        x -= (quad.w * abs(scalarX));
        y -= (quad.h * abs(scalarY));
    }

    int scaledWidth = (int)(view.subWidth * abs(scalarX));
    int scaledHeight = (int)(view.subHeight * abs(scalarY));

    SDL_Rect position =
    {
        (int)x,
        (int)y,
        scaledWidth,
        scaledHeight
    };

    SDL_Point center = {0, 0};

    if (texture != NULL)
    {
        SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(texture, color.a);

        SDL_RenderCopyEx(Window::GetRenderer(), texture, &quad, &position, rotation, &center, this->flip);
    }
}

void Drawable::Flip(double scalarX, double scalarY)
{
    if (scalarX < 0)
    {
        if (this->flip != SDL_FLIP_HORIZONTAL)
            this->flip = SDL_FLIP_HORIZONTAL;
    }
    else if (scalarY < 0)
    {
        if (this->flip != SDL_FLIP_VERTICAL)
            this->flip = SDL_FLIP_VERTICAL;
    }
    else if (scalarX < 0 && scalarY < 0)
    {
        if (this->flip != (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL))
            this->flip = (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
    }
    else
        this->flip = SDL_FLIP_NONE;
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