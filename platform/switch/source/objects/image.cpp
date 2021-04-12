#include "objects/image/image.h"

#include "deko3d/deko.h"

using namespace love;

Image::Image(Data * data) : Texture(Texture::TEXTURE_2D)
{
    // CImage can load PNG and JPG files with the proper libraries
    bool success = this->texture.load(::deko3d::instance().GetImages(), ::deko3d::instance().GetData(),
                                      ::deko3d::instance().GetDevice(), ::deko3d::instance().GetTextureQueue(),
                                      data->GetData(), data->GetSize(), this->width, this->height);

    if (!success)
        throw love::Exception("Failed to upload Image data.");

    this->handle = ::deko3d::instance().RegisterResHandle(this->texture.getDescriptor());

    this->InitQuad();

    this->SetFilter(this->filter);
    this->SetWrap(this->wrap);
}

Image::~Image()
{}

Image::Image(TextureType type, int width, int height) : Texture(type)
{
    this->Init(width, height);

    bool success = this->texture.loadEmptyPixels(::deko3d::instance().GetImages(), ::deko3d::instance().GetData(),
                                                 ::deko3d::instance().GetDevice(), ::deko3d::instance().GetTextureQueue(), 
                                                 width, height, DkImageFormat_RGBA8_Unorm);

    if (!success)
        throw love::Exception("Failed to create Image data");

    this->handle = ::deko3d::instance().RegisterResHandle(this->texture.getDescriptor());

    this->SetFilter(this->filter);
    this->SetWrap(this->wrap);
}

void Image::ReplacePixels(const void * data, size_t size, const Rect & rect)
{
    this->texture.replacePixels(::deko3d::instance().GetData(), ::deko3d::instance().GetDevice(),
                                data, size, ::deko3d::instance().GetTextureQueue(), rect);
}

void Image::Init(int width, int height)
{
    this->width = width;
    this->height = height;

    this->InitQuad();
}
