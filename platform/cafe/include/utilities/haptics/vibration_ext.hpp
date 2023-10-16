#pragma once

#include <utilities/haptics/vibration.tcc>

#include <padscore/wpad.h>
#include <vpad/input.h>

namespace love
{
    template<>
    class Vibration<Console::CAFE> : public Vibration<Console::ALL>
    {
      public:
        Vibration(WPADChan channel);

        Vibration();

        virtual ~Vibration();

        bool SendValues(float left, float right);

        bool Stop();

      private:
        bool isGamepad;
        WPADChan channel;
    };
} // namespace love
