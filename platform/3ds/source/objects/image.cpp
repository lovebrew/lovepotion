#include "objects/image/image.h"
#include "modules/graphics/graphics.h"

using namespace love;

Image::Image(Data * data) : Texture(Texture::TEXTURE_2D)
{
    this->sheet = C2D_SpriteSheetLoadFromMem(data->GetData(), data->GetSize());

    this->texture = C2D_SpriteSheetGetImage(this->sheet, 0);

    this->Init(this->texture.subtex->width, this->texture.subtex->height);

    this->InitQuad();

    this->SetFilter(this->filter);
    this->SetWrap(this->wrap);
}

Image::Image(TextureType type, int width, int height) : Texture(type)
{
    this->Init(width, height);

    this->SetFilter(this->filter);
    this->SetWrap(this->wrap);
}

void Image::Init(int width, int height)
{
    this->width = width;
    this->height = height;
}

Image::~Image()
{
    if (this->sheet != NULL)
        C2D_SpriteSheetFree(this->sheet);
}
