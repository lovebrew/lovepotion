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
}

void Quad::SetSubTexture(Tex3DS_SubTexture * subTexture)
{
    subTexture->top     = 1.0f - this->v0;
    subTexture->left    = this->u0;
    subTexture->right   = this->u1;
    subTexture->bottom  = 1.0f - this->v1;

    subTexture->width   = this->width;
    subTexture->height  = this->height;

    this->subTexture = *subTexture;
}

Tex3DS_SubTexture Quad::GetSubTexture()
{
    return this->subTexture;
}