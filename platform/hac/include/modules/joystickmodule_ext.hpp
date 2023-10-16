#pragma once

#include <modules/joystick/joystickmodule.tcc>

#include <utilities/pool/poolthread.hpp>
#include <utilities/pool/vibrations.hpp>

#include <switch.h>
#include <vector>

namespace love
{
    template<>
    class JoystickModule<Console::HAC> : public JoystickModule<Console::ALL>
    {
      public:
        JoystickModule();

        virtual ~JoystickModule();

        std::vector<HidNpadIdType> AcquireCurrentJoystickIds();

        std::vector<guid::GamepadType> GetActiveStyleSets();

        Joystick<love::Console::Which>* AddJoystick(int index);

        void AddVibration(::Vibration* vibration);

      private:
        VibrationPool* pool;
        PoolThread* thread;
    };
} // namespace love
