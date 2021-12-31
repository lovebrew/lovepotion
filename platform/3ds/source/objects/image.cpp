#include "objects/image/image.h"
#include "modules/graphics/graphics.h"

using namespace love;

Image::Image(const Slices& slices) : Texture(Texture::TEXTURE_2D)
{
    ImageDataBase* data = slices.Get(0, 0);

    this->texture.tex = new C3D_Tex();

    unsigned powTwoWidth  = NextPO2(data->GetWidth());
    unsigned powTwoHeight = NextPO2(data->GetHeight());

    if (!C3D_TexInit(this->texture.tex, powTwoWidth, powTwoHeight, GPU_RGBA8))
        throw love::Exception("Failed to initialize texture!");

    memcpy(this->texture.tex->data, data->GetData(), data->GetSize());
    C3D_TexFlush(this->texture.tex);

    this->Init(data->GetWidth(), data->GetHeight());

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
    this->width  = width;
    this->height = height;
}

Image::~Image()
{}
