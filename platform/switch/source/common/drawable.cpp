#include "common/runtime.h"
#include "common/drawable.h"

#include "modules/window.h"
#include "modules/display.h"

Drawable::Drawable(const string & type) : Object(type)
{
    this->texture = NULL;

    this->flip = SDL_FLIP_NONE;
}

void Drawable::SetFilter()
{
    //stubbed, SDL cannot seem to set individual texture filters
    //Unless I find out it can later™
}

void Drawable::Draw(SDL_Texture * texture, Viewport view, double x, double y, double rotation, double offsetX, double offsetY, double scalarX, double scalarY, SDL_Color color)
{
    if (!texture)
        texture = this->texture;

    double xScaleFactor = 1;
    double yScaleFactor = 1;

    int textureWidth, textureHeight;
    SDL_QueryTexture(texture, NULL, NULL, &textureWidth, &textureHeight);

    xScaleFactor = (double)view.width / (double)textureWidth;
    view.x *= (float)textureWidth / (float)view.width;
    view.subWidth *= (float)textureWidth / (float)view.width;

    yScaleFactor = (float)view.height / (float)textureHeight;
    view.y *= (float)textureHeight / (float)view.height;
    view.subHeight *= (float)textureHeight / (float)view.height;

    double scaledWidth = view.subWidth * xScaleFactor * abs(scalarX);
    double scaledHeight = view.subHeight * yScaleFactor * abs(scalarY);

    if (scalarX < 0)
        x-= scaledWidth;
    if (scalarY < 0)
        y -= scaledHeight;

    SDL_Rect sourceRectangle = {view.x, view.y, view.subWidth, view.subHeight};

    SDL_Rect destinationRectangle =
    {
        (int)round(x - (offsetX * scalarX)),
        (int)round(y - (offsetY * scalarY)),
        (int)ceil(scaledWidth),
        (int)ceil(scaledHeight)
    };

    this->Flip(scalarX, scalarY);

    SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(texture, color.a);

    SDL_Point center = {(int)(offsetX * scalarX), (int)(offsetY * scalarY)};

    SDL_RenderCopyEx(Display::GetRenderer(), texture, &sourceRectangle, &destinationRectangle, rotation, &center, this->flip);
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
