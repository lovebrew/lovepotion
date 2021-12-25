#include "objects/imagedata/imagedatabase.h"

using namespace love;

ImageDataBase::ImageDataBase(PixelFormat format, int width, int height) :
    format(format),
    width(width),
    height(height)
{}

PixelFormat ImageDataBase::GetFormat() const
{
    return this->format;
}

int ImageDataBase::GetWidth() const
{
    return this->width;
}

int ImageDataBase::GetHeight() const
{
    return this->height;
}
