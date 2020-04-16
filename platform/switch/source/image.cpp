#include "common/runtime.h"
#include "objects/image/image.h"

#include "modules/window/window.h"
#include <SDL_image.h>

#define WINDOW_MODULE() (Module::GetInstance<Window>(Module::M_WINDOW))

using namespace love;

Image::Image(const std::string & path) : Texture(Texture::TEXTURE_2D)
{
    SDL_Surface * temp = IMG_Load(path.c_str());

    this->texture = SDL_CreateTextureFromSurface(WINDOW_MODULE()->GetRenderer(), temp);

    SDL_QueryTexture(this->texture, nullptr, nullptr, &this->width, &this->height);
    SDL_SetTextureBlendMode(this->texture, SDL_BLENDMODE_BLEND);

    SDL_FreeSurface(temp);

    this->InitQuad();
}

Image::~Image()
{
    SDL_DestroyTexture(this->texture);
}

void Image::Draw(const DrawArgs & args, const Color & color)
{
    this->Draw(args, this->quad, color);
}

void Image::Draw(const DrawArgs & args, love::Quad * quad, const Color & color)
{
    SDL_SetTextureColorMod(this->texture, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(this->texture, color.a);

    Quad::Viewport v = quad->GetViewport();

    /* do some scaling magic */

    double scaleFactorX = quad->GetTextureWidth() / this->width;
    double scaleSizingX = this->width / quad->GetTextureWidth();

    v.x *= scaleSizingX;
    v.w *= scaleSizingX;

    double scaleFactorY = quad->GetTextureHeight() / this->height;
    double scaleSizingY = this->height / quad->GetTextureHeight();

    v.y *= scaleSizingY;
    v.h *= scaleSizingY;

    double scaledWidth  = v.w * scaleFactorX * std::abs(args.scalarX);
    double scaledHeight = v.h * scaleFactorY * std::abs(args.scalarY);

    /* adjust position  and flip mode*/

    float x = args.x;
    float y = args.y;

    if (args.scalarX < 0)
        x -= scaledWidth;
    else if (args.scalarY < 0)
        y -= scaledHeight;

    SDL_RendererFlip flipHorizonal = (args.scalarX < 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RendererFlip flipVertical  = (args.scalarY < 0) ? SDL_FLIP_VERTICAL   : SDL_FLIP_NONE;

    /* render */

    SDL_Rect source = {v.x, v.y, v.w, v.h};
    SDL_Rect destin =
    {
        (int)round(x - (args.offsetX * args.scalarX)),
        (int)round(y - (args.offsetY * args.scalarY)),
        (int)ceil(scaledWidth),
        (int)ceil(scaledHeight)
    };

    SDL_Point center = {(int)(args.offsetX * args.scalarX), (int)(args.offsetY * args.scalarY)};
    double rotation = (args.r * 180 / M_PI);

    SDL_RenderCopyEx(WINDOW_MODULE()->GetRenderer(), this->texture, &source, &destin, rotation, &center, (SDL_RendererFlip)(flipVertical | flipHorizonal));
}
