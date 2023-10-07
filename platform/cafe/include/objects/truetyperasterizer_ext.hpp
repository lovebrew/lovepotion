#pragma once

#include <common/strongreference.hpp>

#include <objects/truetyperasterizer/truetyperasterizer.tcc>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

namespace love
{
    template<>
    class TrueTypeRasterizer<Console::CAFE> : public TrueTypeRasterizer<Console::ALL>
    {
      public:
        using TrueTypeRasterizer<Console::ALL>::TrueTypeRasterizer;
    };
} // namespace love
