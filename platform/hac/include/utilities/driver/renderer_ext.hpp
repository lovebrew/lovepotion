#pragma once

#include <utilities/driver/renderer/renderer.tcc>

#include <switch.h>

namespace love
{
    template<>
    class Renderer<Console::HAC> : public Renderer<Console::ALL>
    {
    };
} // namespace love
