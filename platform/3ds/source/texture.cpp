#include "common/runtime.h"
#include "objects/texture/texture.h"

#include "modules/graphics/graphics.h"

using namespace love;

void Texture::Draw(Graphics * gfx, const Matrix4 & localTransform)
{
    this->Draw(gfx, this->quad, localTransform);
}

void Texture::Draw(Graphics * gfx, love::Quad * quad, const Matrix4 & localTransform)
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

    this->texture.subtex = &source;

    // Multiply the current and local transforms
    Matrix4 t(gfx->GetTransform(), localTransform);

    C2D_DrawParams params;

    params.pos = {0.0f, 0.0f, v.w, v.h};
    params.depth = Graphics::CURRENT_DEPTH;
    params.angle = 0.0f;
    params.center = {0.0f, 0.0f};

    // Appy the new Matrix4 C3D_Mtx
    C2D_ViewRestore(&t.GetElements());

    C2D_ImageTint tint;
    Color color = gfx->GetColor();

    C2D_AlphaImageTint(&tint, color.a);

    C2D_DrawImage(this->texture, &params, &tint);
}