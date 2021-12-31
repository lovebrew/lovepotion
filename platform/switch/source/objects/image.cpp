#include "objects/image/image.h"

#include "deko3d/deko.h"

using namespace love;

Image::Image(const Slices& slices) : Texture(Texture::TEXTURE_2D)
{
    ImageDataBase* slice = slices.Get(0, 0);

    this->texture.load(slice->GetData(), slice->GetSize(), slice->GetWidth(), slice->GetHeight());

    this->width  = slice->GetWidth();
    this->height = slice->GetHeight();

    this->handle = ::deko3d::Instance().RegisterResHandle(this->texture.getDescriptor());

    this->InitQuad();

    this->SetFilter(this->filter);
    this->SetWrap(this->wrap);
}

Image::~Image()
{}

Image::Image(TextureType type, int width, int height) : Texture(type)
{
    this->Init(width, height);

    bool success = this->texture.loadEmptyPixels(
        ::deko3d::Instance().GetImages(), ::deko3d::Instance().GetData(),
        ::deko3d::Instance().GetDevice(), ::deko3d::Instance().GetTextureQueue(), width, height,
        DkImageFormat_RGBA8_Unorm);

    if (!success)
        throw love::Exception("Failed to create Image data");

    this->handle = ::deko3d::Instance().RegisterResHandle(this->texture.getDescriptor());

    this->SetFilter(this->filter);
    this->SetWrap(this->wrap);
}

void Image::ReplacePixels(const void* data, size_t size, const Rect& rect)
{
    this->texture.replacePixels(::deko3d::Instance().GetData(), ::deko3d::Instance().GetDevice(),
                                data, size, ::deko3d::Instance().GetTextureQueue(), rect);
}

void Image::Init(int width, int height)
{
    this->width  = width;
    this->height = height;

    this->InitQuad();
}
