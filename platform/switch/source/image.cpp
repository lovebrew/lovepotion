#include "common/runtime.h"
#include "objects/image/image.h"

#include "modules/window/window.h"

#define WINDOW_MODULE() (Module::GetInstance<Window>(Module::M_WINDOW))

using namespace love;

Image::Image(Data * data) : Texture(Texture::TEXTURE_2D)
{
    // SDL_Surface * temp = IMG_Load_RW(SDL_RWFromMem(data->GetData(), data->GetSize()), 1);

    // this->texture = SDL_CreateTextureFromSurface(WINDOW_MODULE()->GetRenderer(), temp);

    // SDL_QueryTexture(this->texture, nullptr, nullptr, &this->width, &this->height);
    // SDL_SetTextureBlendMode(this->texture, SDL_BLENDMODE_BLEND);

    // SDL_FreeSurface(temp);

    // this->InitQuad();
}

Image::~Image()
{
    // SDL_DestroyTexture(this->texture);
}
