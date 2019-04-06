#include "common/runtime.h"
#include "common/drawable.h"
#include "modules/graphics.h"

Drawable::Drawable(const string & type) : Object(type) {}

void Drawable::Draw(float x, float y, float rotation, float scalarX, float scalarY, Color color)
{
    C2D_AlphaImageTint(&imageTint, color.a);

    if (rotation == 0)
        C2D_DrawImageAt(this->image, x, y, 0.5, &imageTint, scalarX, scalarY);
    else
        C2D_DrawImageAtRotated(this->image, x, y, 0.5f, rotation, &imageTint, scalarX, scalarY);
}

int Drawable::GetWidth()
{
    return this->width;
}

int Drawable::GetHeight()
{
    return this->height;
}

Tex3DS_SubTexture Drawable::GetSubTexture()
{
    return this->subTexture;
}

void Drawable::SetSubTexture(const Tex3DS_SubTexture & subTexture)
{
    this->image.subtex = &subTexture;
}
