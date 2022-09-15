#pragma once

#include <modules/joystick/joystickmodule.tcc>

namespace love
{
    template<>
    class JoystickModule<Console::CAFE> : public JoystickModule<Console::ALL>
    {
    };
} // namespace love
