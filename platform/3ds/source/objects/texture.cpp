#include "citro2d/citro.h"

#include "modules/graphics/graphics.h"
#include "objects/texture/texture.h"

using namespace love;

Texture::Texture(TextureType type) : common::Texture(TEXTURE_2D)
{}

void Texture::Draw(Graphics* gfx, const Matrix4& localTransform)
{
    this->Draw(gfx, this->quad, localTransform);
}

const C2D_Image& Texture::GetHandle()
{
    return this->texture;
}

void Texture::SetSamplerState(const SamplerState& state)
{
    this->samplerState = state;

    if (state.mipmapFilter != SamplerState::MIPMAP_FILTER_NONE && this->GetMipmapCount() == 1)
        this->samplerState.mipmapFilter = SamplerState::MIPMAP_FILTER_NONE;

    ::citro2d::Instance().SetSamplerState(this, this->samplerState);
}

void Texture::Draw(Graphics* gfx, love::Quad* quad, const Matrix4& localTransform)
{
    Quad::Viewport v = quad->GetViewport();

    Tex3DS_SubTexture tv = quad->CalculateTex3DSViewport(v, this->texture.tex);
    this->texture.subtex = &tv;

    // Multiply the current and local transforms
    Matrix4 t(gfx->GetTransform(), localTransform);

    C2D_DrawParams params;

    params.pos    = { 0.0f, 0.0f, (float)v.w, (float)v.h };
    params.depth  = Graphics::CURRENT_DEPTH;
    params.angle  = 0.0f;
    params.center = { 0.0f, 0.0f };

    C2D_ViewRestore(&t.GetElements());

    C2D_ImageTint tint;
    Colorf color = gfx->GetColor();

    C2D_PlainImageTint(&tint, C2D_Color32f(color.r, color.g, color.b, color.a), 1);
    C2D_DrawImage(this->texture, &params, &tint);
}
