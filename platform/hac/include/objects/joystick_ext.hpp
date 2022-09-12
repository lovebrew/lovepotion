#pragma once

#include <objects/joystick/joystick.tcc>

namespace love
{
    template<>
    class Joystick<Console::HAC> : public Joystick<Console::ALL>
    {
    };
} // namespace love
