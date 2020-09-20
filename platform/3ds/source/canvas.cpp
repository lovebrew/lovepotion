#include "common/runtime.h"
#include "objects/canvas/canvas.h"

#include "modules/window/window.h"

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
    this->subHandle = {&this->citroTex, &subtex};

    // Create Sprite (Texture) from Image
    C2D_SpriteFromImage(&this->texture, this->subHandle);

    this->cleared = false;

    this->InitQuad();
}

Canvas::~Canvas()
{
    C3D_TexDelete(&this->citroTex);
    C3D_RenderTargetDelete(this->renderer);
}

void Canvas::Draw(const DrawArgs & args, const Color & color)
{
    this->Draw(args, this->quad, color);
}

void Canvas::Draw(const DrawArgs & args, love::Quad * quad, const Color & color)
{
    Quad::Viewport v = quad->GetViewport();
    Tex3DS_SubTexture source;

    /* Set up the Quad */

    int w =  quad->GetTextureWidth(true);
    int h =  quad->GetTextureHeight(true);

    v.x += 1;
    v.y += 1;

    v.w -= 2;
    v.h -= 2;

    source.top    = 1.0f - (v.y / h);
    source.left   = (v.x / w);

    source.right  = (v.x + v.w) / w;
    source.bottom = 1.0f - ((v.y + v.h) / h);

    source.width  = v.w;
    source.height = v.h;

    this->texture.image.subtex = &source;

    /*
    ** Note that the offsets are applied before rotation, scaling, or shearing;
    ** Scaling and shearing are applied before rotation.
    **
    ** Offset X and Y (aka centering values) always push the texture left and up
    ** as if they were negative
    */

    this->texture.params.pos.w = v.w;
    this->texture.params.pos.h = v.h;

    C2D_SpriteSetPos(&this->texture, args.x, args.y);
    C2D_SpriteSetDepth(&this->texture, args.depth);

    C2D_SpriteSetCenterRaw(&this->texture, args.offsetX, args.offsetY);
    C2D_SpriteSetScale(&this->texture, args.scalarX, args.scalarY);

    C2D_SpriteSetRotation(&this->texture, args.r);

    C2D_ImageTint tint;
    C2D_AlphaImageTint(&tint, color.a);

    /* Render the texture */
    C2D_DrawSpriteTinted(&this->texture, &tint);
}

void Canvas::Clear(const Color & color)
{
    if (this->cleared)
        return;

    C2D_TargetClear(this->renderer, C2D_Color32f(color.r, color.g, color.b, color.a));

    this->cleared = true;
}