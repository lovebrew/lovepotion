#include "common/config.hpp"

#include "driver/EventQueue.hpp"
#include "modules/joystick/JoystickModule.hpp"

#include <cstdint>
#include <padscore/kpad.h>
#include <padscore/wpad.h>
#include <vpad/input.h>

#include "modules/joystick/kpad/Joystick.hpp"
#include "modules/joystick/vpad/Joystick.hpp"

namespace love::joystick
{
    void connectCallback(WPADChan channel, WPADError error)
    {
        const auto status = error == WPAD_ERROR_NONE;
        EventQueue::getInstance().sendJoystickStatus(status, channel + 1);
    }

    void init()
    {
        for (size_t channel = WPAD_CHAN_0; channel < KPADGetMaxControllers(); channel++)
            KPADSetConnectCallback((KPADChan)channel, connectCallback);
    }

    int getJoystickCount()
    {
        size_t count = 0;

        {
            VPADStatus status {};
            VPADReadError error = VPAD_READ_SUCCESS;

            VPADRead(VPAD_CHAN_0, &status, 1, &error);

            if (error == VPAD_READ_SUCCESS || error == VPAD_READ_NO_SAMPLES)
                count++;
        }

        for (uint32_t channel = 0; channel < KPADGetMaxControllers(); channel++)
        {
            auto extension   = WPAD_EXT_UNKNOWN;
            const auto error = WPADProbe((WPADChan)channel, &extension);
            if (error == WPAD_ERROR_NONE && extension != WPAD_EXT_UNKNOWN)
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
