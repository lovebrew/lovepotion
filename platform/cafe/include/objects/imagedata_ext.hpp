#pragma once

#include <objects/imagedata/imagedata.tcc>

namespace love
{
    template<>
    class ImageData<Console::CAFE> : public ImageData<Console::ALL>
    {
      public:
        using ImageData<Console::ALL>::ImageData;

        ImageData<Console::CAFE>* Clone() const override
        {
            return new ImageData<Console::CAFE>(*this);
        }
    };
} // namespace love
