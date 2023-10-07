#pragma once

#include <objects/truetyperasterizer/truetyperasterizer.tcc>

namespace love
{
    template<>
    class TrueTypeRasterizer<Console::HAC> : public TrueTypeRasterizer<Console::ALL>
    {
      public:
        using TrueTypeRasterizer<Console::ALL>::TrueTypeRasterizer;
    };
} // namespace love
