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

        std::vector<WPADChan> AcquireCurrentJoystickIds();

        ::Joystick* AddJoystick(int index);
    };
} // namespace love
