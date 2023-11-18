#pragma once

#include <common/matrix.hpp>

namespace love
{
    template<>
    class Matrix4<Console::CAFE> : public Matrix4<Console::ALL>
    {
        using Matrix4<Console::ALL>::Matrix4;
    }
} // namespace love
