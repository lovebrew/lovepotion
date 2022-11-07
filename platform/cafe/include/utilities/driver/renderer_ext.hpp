#pragma once

#include <utilities/driver/renderer/renderer.tcc>

namespace love
{
    template<>
    class Renderer<Console::CAFE> : public Renderer<Console::ALL>
    {
    };
} // namespace love
