#include "common/runtime.h"

#include "objects/image/image.h"
#include "modules/graphics/graphics.h"

using namespace love;

Image::Image(Data * data) : Texture(Texture::TEXTURE_2D)
{
    this->sheet = C2D_SpriteSheetLoadFromMem(data->GetData(), data->GetSize());

    this->texture = C2D_SpriteSheetGetImage(this->sheet, 0);

    this->width  = this->texture.subtex->width;
    this->height = this->texture.subtex->height;

    this->InitQuad();
}

Image::~Image()
{
    if (this->sheet != NULL)
        C2D_SpriteSheetFree(this->sheet);
}
