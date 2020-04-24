#include "common/runtime.h"
#include "objects/canvas/canvas.h"

#include "modules/window/window.h"

using namespace love;

love::Type Canvas::type("Canvas", &Texture::type);

#define WINDOW_MODULE() (Module::GetInstance<Window>(Module::M_WINDOW))

Canvas::Canvas(const Canvas::Settings & settings) : Texture(TextureType::TEXTURE_2D)
{
    this->width = settings.width;
    this->height = settings.height;

    this->texture = SDL_CreateTexture(WINDOW_MODULE()->GetRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);

    /* Set blend mode and clear to transparent black */

    SDL_SetTextureBlendMode(this->texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureColorMod(this->texture, 0, 0, 0);
    SDL_SetTextureAlphaMod(this->texture, 0);

    canvasCount++;

    this->InitQuad();
}

void Canvas::Draw(const DrawArgs & args, love::Quad * quad, const Color & color)
{

}

Canvas::~Canvas()
{
    canvasCount--;
}
