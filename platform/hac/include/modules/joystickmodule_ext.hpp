#pragma once

#include <modules/joystick/joystickmodule.tcc>

#include <switch.h>

namespace love
{
    template<>
    class JoystickModule<Console::HAC> : public JoystickModule<Console::ALL>
    {
      public:
        JoystickModule();

        int GetCurrentJoystickCount(HidsysUniquePadId* ids = nullptr);

        ::Joystick* AddJoystick(int index);

      private:
        static constexpr MAX_JOYSTICKS = 0x08;
    };
} // namespace love
