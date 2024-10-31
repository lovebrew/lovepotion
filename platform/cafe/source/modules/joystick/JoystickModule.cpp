#include "modules/joystick/JoystickModule.hpp"

#include <padscore/kpad.h>
#include <vpad/input.h>

#include "modules/joystick/kpad/Joystick.hpp"
#include "modules/joystick/vpad/Joystick.hpp"

namespace love::joystick
{
    int getJoystickCount()
    {
        int count = 0;

        VPADStatus vpadStatus {};
        VPADReadError error = VPAD_READ_SUCCESS;

        VPADRead(VPAD_CHAN_0, &vpadStatus, 1, &error);

        if (error == VPAD_READ_SUCCESS || error == VPAD_READ_NO_SAMPLES)
            count++;

        for (int channel = 0; channel < 4; channel++)
        {
            KPADStatus kpadStatus {};
            KPADError error = KPAD_ERROR_OK;

            KPADReadEx((KPADChan)channel, &kpadStatus, 1, &error);
            bool success = error == KPAD_ERROR_OK || error == KPAD_ERROR_NO_SAMPLES;

            if (success && kpadStatus.extensionType != 0xFF)
                count++;
        }

        return count;
    }

    JoystickBase* openJoystick(int index)
    {
        if (index == 0)
            return new vpad::Joystick(index);

        return new kpad::Joystick(index);
    }
} // namespace love::joystick
