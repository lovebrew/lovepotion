#include "common/runtime.h"
#include "objects/image/image.h"

#include "deko3d/deko.h"

using namespace love;

Image::Image(Data * data) : Texture(Texture::TEXTURE_2D)
{
    // CImage can load PNG and JPG files with the proper libraries
    void * buffer = this->texture.load(data->GetData(), data->GetSize(),
                                       this->width, this->height);

    if (buffer == nullptr)
        throw love::Exception("Failed to load Image data.");

    bool success = this->texture.loadMemory(*dk3d.GetImages(), *dk3d.GetData(), dk3d.GetDevice(), dk3d.GetTextureQueue(),
                                            buffer, this->width, this->height, DkImageFormat_RGBA8_Unorm);

    if (!success)
        throw love::Exception("Failed to upload Image data.");

    free(buffer);

    dk3d.RegisterResHandle(this->texture.getDescriptor(), this);

    this->InitQuad();
}

Image::Image(TextureType type, int width, int height) : Texture(type)
{
    this->Init(width, height);

    this->texture.loadEmptyPixels(*dk3d.GetImages(), *dk3d.GetData(), dk3d.GetDevice(), dk3d.GetTextureQueue(),
                                   width, height, DkImageFormat_RGBA8_Unorm);

    dk3d.RegisterResHandle(this->texture.getDescriptor(), this);
}

void Image::ReplacePixels(const void * data, size_t size, const Rect & rect)
{
    this->texture.replacePixels(*dk3d.GetData(), dk3d.GetDevice(), data, size, dk3d.GetTextureQueue(), rect);
}

void Image::Init(int width, int height)
{
    this->width = width;
    this->height = height;

    this->InitQuad();
}