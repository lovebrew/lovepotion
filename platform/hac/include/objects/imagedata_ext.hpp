#pragma once

#include <objects/imagedata/imagedata.tcc>

namespace love
{
    template<>
    class ImageData<Console::HAC> : public ImageData<Console::ALL>
    {
      public:
        using ImageData<Console::ALL>::ImageData;

        ImageData<Console::HAC>* Clone() const override
        {
            return new ImageData<Console::HAC>(*this);
        }
    };
} // namespace love
