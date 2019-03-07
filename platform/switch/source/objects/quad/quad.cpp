#include "common/runtime.h"
#include "objects/quad/quad.h"

Quad::Quad(float subTextureX, float subTextureY, float subTextureWidth, float subTextureHeight, float atlasWidth, float atlasHeight) : Object("Quad")
{
    this->view.x = subTextureX;
    this->view.y = subTextureY;

    this->view.subWidth = subTextureWidth;
    this->view.subHeight = subTextureHeight;

    this->view.width = atlasWidth;
    this->view.height = atlasHeight;
}

void Quad::SetViewport(int x, int y, int width, int height)
{
    this->view.x = x;
    this->view.y = y;

    this->view.subWidth = width;
    this->view.subHeight = height;
}

Viewport Quad::GetViewport()
{
    return this->view;
}

void Quad::GetTextureDimensions(int width, int height)
{
    width = this->view.width;
    height = this->view.height;
}