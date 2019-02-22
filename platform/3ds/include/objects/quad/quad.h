#pragma once

class Quad : public Object
{
    public:
        Quad() {};
        Quad(float subTextureX, float subTextureY, float subTextureWidth, float subTextureHeight, float atlasWidth, float atlasHeight);

        void SetSubTexture(Tex3DS_SubTexture * subTexture);
        Tex3DS_SubTexture GetSubTexture();

    private:
        float width;
        float height;

        float u0;
        float v0;

        float u1;
        float v1;

        Tex3DS_SubTexture subTexture;
};