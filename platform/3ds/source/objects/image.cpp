#include "objects/image/image.h"
#include "modules/graphics/graphics.h"

using namespace love;

Image::Image(const Slices& slices) : Image(slices, true)
{
    ImageDataBase* slice = slices.Get(0, 0);
    this->Init(slice->GetFormat(), slice->GetWidth(), slice->GetHeight());
}

Image::Image(const Slices& slices, bool validate) :
    Texture(data.GetTextureType()),
    data(slices),
    mipmapsType(MIPMAPS_NONE),
    sRGB(false)
{
    if (validate && this->data.Validate() == MIPMAPS_DATA)
        mipmapsType = MIPMAPS_DATA;
}

Image::Image(TextureType type, PixelFormat format, int width, int height, int slices) :
    Image(Slices(type), false)
{
    this->Init(format, width, height);

    this->SetFilter(this->filter);
    this->SetWrap(this->wrap);
}

void Image::Init(PixelFormat format, int width, int height)
{
    this->texture.tex = new C3D_Tex();

    unsigned powTwoWidth  = NextPO2(width + 2);
    unsigned powTwoHeight = NextPO2(height + 2);

    if (!C3D_TexInit(this->texture.tex, powTwoWidth, powTwoHeight, GPU_RGBA8))
        throw love::Exception("Failed to initialize texture!");

    size_t copySize = powTwoWidth * powTwoHeight * 4;
    memcpy(this->texture.tex->data, this->data.Get(0, 0)->GetData(), copySize);

    C3D_TexFlush(this->texture.tex);

    this->format = format;

    this->width  = width;
    this->height = height;

    this->InitQuad();

    this->SetFilter(this->filter);
    this->SetWrap(this->wrap);
}

Image::~Image()
{
    C3D_TexDelete(this->texture.tex);
    delete this->texture.tex;
}
