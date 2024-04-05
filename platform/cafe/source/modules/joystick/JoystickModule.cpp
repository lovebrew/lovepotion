#include "modules/joystick/JoystickModule.hpp"

#include <padscore/kpad.h>
#include <vpad/input.h>

namespace love::joystick
{
    int getJoystickCount()
    {
        int count = 0;

        VPADStatus status {};
        VPADReadError error = VPAD_READ_SUCCESS;

        VPADRead(VPAD_CHAN_0, &status, 1, &error);

        if (error == VPAD_READ_SUCCESS)
            count++;

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

    /*
    ** TODO: open either vpad::Joystick or kpad::Joystick based on what opens first.
    */
    JoystickBase* openJoystick(int index)
    {
        return nullptr;
    }
} // namespace love::joystick
