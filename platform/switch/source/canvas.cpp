#include "common/runtime.h"
#include "objects/canvas/canvas.h"

#include "modules/window/window.h"

using namespace love;

#define WINDOW_MODULE() (Module::GetInstance<Window>(Module::M_WINDOW))

Canvas::Canvas(const Canvas::Settings & settings) : Texture(TextureType::TEXTURE_2D)
{
    this->width = settings.width;
    this->height = settings.height;

    // this->texture = SDL_CreateTexture(WINDOW_MODULE()->GetRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);

    // /* Set blend mode and clear to transparent black */

    // SDL_SetTextureBlendMode(this->texture, SDL_BLENDMODE_BLEND);

    // this->cleared = false;

    this->InitQuad();
}

Canvas::~Canvas()
{
    // SDL_DestroyTexture(this->texture);
}

void Canvas::Clear(const Colorf & color)
{
    // SDL_SetRenderDrawColor(WINDOW_MODULE()->GetRenderer(), color.r, color.g, color.b, color.a);
    // SDL_RenderClear(WINDOW_MODULE()->GetRenderer());

    // if (!this->cleared)
    //     this->cleared = true;
}