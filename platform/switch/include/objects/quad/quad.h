#pragma once

class Quad : public Object
{
    public:
        Quad() {};
        Quad(float subTextureX, float subTextureY, float subTextureWidth, float subTextureHeight, float atlasWidth, float atlasHeight);
        
        Viewport GetViewport();
        void SetViewport(int x, int y, int width, int height);
        void GetTextureDimensions(int width, int height);
    
    private:
        int NextPow2(unsigned int x);
        Viewport view;
};