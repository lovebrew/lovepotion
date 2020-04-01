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

    SDL_Rect source = {v.x, v.y, v.w, v.h};
    SDL_Rect destin = {args.x, args.y, v.w, v.h};

    SDL_RenderCopyEx(WINDOW_MODULE()->GetRenderer(), this->texture, &source, &destin, 0.0, nullptr, SDL_RendererFlip::SDL_FLIP_NONE);
}
