#include "common/runtime.h"
#include "objects/canvas/canvas.h"

#include "modules/window/window.h"
#include "modules/graphics/graphics.h"

using namespace love;

#define WINDOW_MODULE() (Module::GetInstance<Window>(Module::M_WINDOW))

Canvas::Canvas(const Canvas::Settings & settings) : Texture(TextureType::TEXTURE_2D)
{
    this->width  = settings.width;
    this->height = settings.height;

    C3D_TexInitVRAM(&this->citroTex, NextPO2(this->width), NextPO2(this->height), GPU_RGBA8);

    this->renderer = C3D_RenderTargetCreateFromTex(&this->citroTex, GPU_TEXFACE_2D, 0, -1);

    const Tex3DS_SubTexture subtex = {
        this->citroTex.width, this->citroTex.height,
        0.0f, 1.0f, 1.0f, 0.0f
    };

    // C2D_Image
    this->texture = {&this->citroTex, &subtex};

    this->cleared = false;

    this->InitQuad();
}

Canvas::~Canvas()
{
    C3D_TexDelete(&this->citroTex);
    C3D_RenderTargetDelete(this->renderer);
}

void Canvas::Clear(const Colorf & color)
{
    C2D_TargetClear(this->renderer, C2D_Color32f(color.r, color.g, color.b, color.a));

    if (!this->cleared)
        this->cleared = true;
}