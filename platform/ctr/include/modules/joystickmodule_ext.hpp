#pragma once

#include <modules/joystick/joystickmodule.tcc>

namespace love
{
    template<>
    class JoystickModule<Console::CTR> : public JoystickModule<Console::ALL>
    {
      public:
        JoystickModule();

        int GetCurrentJoystickCount() const
        {
            return 1;
        }

        Joystick<Console::Which>* AddJoystick(int index);
    };
} // namespace love
