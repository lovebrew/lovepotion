#pragma once

#include <objects/imagedata/imagedata.tcc>

namespace love
{
    template<>
    class ImageData<Console::CAFE> : public ImageData<Console::ALL>
    {
    };
} // namespace love
