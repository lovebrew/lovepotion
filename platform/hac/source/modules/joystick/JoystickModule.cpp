#include "modules/joystick/JoystickModule.hpp"

#include <switch.h>

namespace love::joystick
{
    // clang-format off
    static constexpr std::array players =
    {
        HidNpadIdType_Handheld,
        HidNpadIdType_No1,
        HidNpadIdType_No2,
        HidNpadIdType_No3,
        HidNpadIdType_No4,
        HidNpadIdType_No5,
        HidNpadIdType_No6,
        HidNpadIdType_No7,
        HidNpadIdType_No8
    };
    // clang-format on

    int getJoystickCount()
    {
        int count = 0;

        for (size_t i = 0; i < players.size(); i++)
        {
            if (hidGetNpadStyleSet(players[i]) != 0)
                count++;
        }

        return count;
    }

    JoystickBase* openJoystick(int index)
    {
        return new Joystick(index);
    }
} // namespace love::joystick
