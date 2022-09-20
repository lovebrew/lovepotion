#pragma once

#include <modules/joystick/joystickmodule.tcc>

namespace love
{
    template<>
    class JoystickModule<Console::CAFE> : public JoystickModule<Console::ALL>
    {
      public:
        JoystickModule();

        virtual ~JoystickModule();

        ::Joystick* AddJoystick(int index);
    };
} // namespace love
