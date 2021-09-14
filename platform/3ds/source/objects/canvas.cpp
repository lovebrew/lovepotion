#include "objects/canvas/canvas.h"

#include "citro2d/citro.h"
#include "modules/graphics/graphics.h"
#include "modules/window/window.h"

using namespace love;

Canvas::Canvas(const Canvas::Settings& settings) : common::Canvas(settings)
{
    C3D_TexInitVRAM(&this->citroTex, NextPO2(this->width), NextPO2(this->height), GPU_RGBA8);

    this->renderer =
        C3D_RenderTargetCreateFromTex(&this->citroTex, GPU_TEXFACE_2D, 0, GPU_RB_DEPTH16);

    u16 width  = this->citroTex.width;
    u16 height = this->citroTex.height;

    this->subtex = { width, height, 0.0f, 1.0f, 1.0f, 0.0f };

    // C2D_Image
    this->texture = { &this->citroTex, &this->subtex };

    ::citro2d::Instance().BindFramebuffer(this);
    ::citro2d::Instance().ClearColor({ 0, 0, 0, 0 });
    ::citro2d::Instance().BindFramebuffer();

    this->InitQuad();
}

Canvas::~Canvas()
{
    ::citro2d::Instance().DeferCallToEndOfFrame(([target = renderer, tex = citroTex]() mutable {
        C3D_RenderTargetDelete(target);
        C3D_TexDelete(&tex);
    }));
}

C3D_RenderTarget* Canvas::GetRenderer()
{
    return this->renderer;
}

void Canvas::Draw(Graphics* gfx, Quad* quad, const Matrix4& localTransform)
{
    if (gfx->IsCanvasActive(this))
        throw love::Exception("Cannot render a Canvas to itself!");

    Texture::Draw(gfx, quad, localTransform);
}

void Canvas::Clear(const Colorf& color)
{
    C2D_TargetClear(this->renderer, C2D_Color32f(color.r, color.g, color.b, color.a));

    if (!this->cleared)
        this->cleared = true;
}
