#include "common/runtime.h"
#include "objects/image/image.h"

#include "modules/window/window.h"
#include <SDL_image.h>

using namespace love;

Image::Image(const std::string & path)
{
    SDL_Surface * temp = IMG_Load(path.c_str());

    auto window = Module::GetInstance<Window>(Module::M_WINDOW);
    this->texture = SDL_CreateTextureFromSurface(window->GetRenderer(), temp);

    SDL_FreeSurface(temp);

    SDL_QueryTexture(this->texture, nullptr, nullptr, &this->width, &this->height);
    SDL_SetTextureBlendMode(this->texture, SDL_BLENDMODE_BLEND);
}

void Image::Draw(const DrawArgs & args, const Color & color)
{
    SDL_SetTextureColorMod(this->texture, color.r, color.g, color.g);
    SDL_SetTextureAlphaMod(this->texture, color.a);

    auto window = Module::GetInstance<Window>(Module::M_WINDOW);

    SDL_Rect source({0, 0, this->width, this->height});
    SDL_Rect destin({args.x, args.y, this->width, this->height});

    SDL_RenderCopyEx(window->GetRenderer(), this->texture, &source, &destin, 0.0, nullptr, SDL_RendererFlip::SDL_FLIP_NONE);
}
