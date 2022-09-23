#include <utilities/haptics/vibration_ext.hpp>
#include <utilities/result.hpp>

#include <vpad/input.h>

#include <algorithm>

using namespace love;

Vibration<Console::CAFE>::Vibration() : isGamepad(true)
{}

Vibration<Console::CAFE>::Vibration(WPADChan channel) : isGamepad(false)
{
    this->channel = channel;
}

Vibration<Console::CAFE>::~Vibration()
{
    this->Stop();
}

bool Vibration<Console::CAFE>::Stop()
{
    return this->SendValues(0, 0);
}

bool Vibration<Console::CAFE>::SendValues(float left, float right)
{
    /* success */
    int32_t result = 0;

    if (this->isGamepad)
    {
        uint8_t average = ((left + right) * 0.5) * 0xFF;
        uint8_t buffer[0x0F] { 0 };

        if (average == 0)
            VPADStopMotor(VPAD_CHAN_0);
        else
        {
            std::fill_n(buffer, sizeof(buffer), average);
            result = VPADControlMotor(VPAD_CHAN_0, buffer, sizeof(buffer));
        }
    }
    else
    {
        bool shouldRumble = (left != 0.0f || right != 0.0f);
        WPADControlMotor(this->channel, shouldRumble);
    }

    if (ResultCode(result).Success())
    {
        this->vibrationInfo.left  = left;
        this->vibrationInfo.right = right;
    }
    else
    {
        this->vibrationInfo.left = this->vibrationInfo.right = 0.0f;
        this->vibrationInfo.endTime                          = -1.0f;
    }

    return ResultCode(result).Success();
}
