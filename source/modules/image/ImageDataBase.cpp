#include "modules/image/ImageDataBase.hpp"

namespace love
{
    ImageDataBase::ImageDataBase(PixelFormat format, int width, int height) :
        format(format),
        width(width),
        height(height),
        linear(false)
    {}
} // namespace love
