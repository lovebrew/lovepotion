#include "common/runtime.h"
#include "objects/quad/quad.h"

Quad::Quad(float subTextureX, float subTextureY, float subTextureWidth, float subTextureHeight, float atlasWidth, float atlasHeight) : Object("Quad")
{
    int w_pow2 = NextPO2(atlasWidth);
    int h_pow2 = NextPO2(atlasHeight);

    this->u0 = subTextureX/w_pow2;
    this->v0 = subTextureY/h_pow2;

    this->u1 = (subTextureX + subTextureWidth)/w_pow2;
    this->v1 = (subTextureY + subTextureHeight)/h_pow2;

    this->width = subTextureWidth;
    this->height = subTextureHeight;

    this->FillSubTextureData(&this->subTexture);
}

void Quad::FillSubTextureData(Tex3DS_SubTexture * subTex)
{
    subTex->top     = 1.0f - this->v0;
    subTex->left    = this->u0;
    subTex->right   = this->u1;
    subTex->bottom  = 1.0f - this->v1;

    subTex->width   = this->width;
    subTex->height  = this->height;
}

Tex3DS_SubTexture * Quad::GetSubTexture()
{
    return &this->subTexture;
}