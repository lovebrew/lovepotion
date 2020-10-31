#include "common/runtime.h"
#include "objects/image/image.h"

#include "deko3d/deko.h"

using namespace love;

Image::Image(Data * data) : Texture(Texture::TEXTURE_2D)
{
    // CImage can load PNG and JPG files with the proper libraries
    u32 * buffer = this->texture.loadPNG(data->GetData(), data->GetSize(), this->width, this->height);

    // Have the deko3d class load our new buffer and register it to the CDescriptorSets
    dk3d.LoadTextureBuffer(this->texture, buffer, this->width * this->height * 4, this, DkImageFormat_RGBA8_Unorm);

    this->InitQuad();
}

Image::~Image()
{
    dk3d.UnRegisterResHandle(this);
}
