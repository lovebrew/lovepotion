#pragma once

#include <modules/joystick/joystickmodule.tcc>

#include <utilities/pool/poolthread.hpp>
#include <utilities/pool/vibrations.hpp>

namespace love
{
    template<>
    class JoystickModule<Console::CAFE> : public JoystickModule<Console::ALL>
    {
      public:
        JoystickModule();

        virtual ~JoystickModule();

        Joystick<Console::Which>* AddJoystick(int index);

        void AddVibration(::Vibration* vibration);

      private:
        VibrationPool* pool;
        PoolThread* thread;
    };
} // namespace love
