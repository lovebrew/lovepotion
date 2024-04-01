#include "modules/joystick/JoystickModule.hpp"

#include <padscore/kpad.h>

namespace love::joystick
{
    /**
     * The Wii U Gamepad is always connected.
     * So the count starts at 1.
     */
    int getJoystickCount()
    {
        int count = 1;

        for (int channel = 0; channel < 4; channel++)
        {
            KPADStatus status {};
            KPADError error = KPAD_ERROR_OK;

            KPADReadEx((KPADChan)channel, &status, 1, &error);

            if (error == KPAD_ERROR_OK && status.extensionType != 0xFF)
                count++;
        }

        return count;
    }
} // namespace love::joystick
