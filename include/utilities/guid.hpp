#pragma once

#include <objects/joystick/joystick.tcc>

#include <vector>

namespace love::guid
{
    enum GamepadType
    {
        GAMEPAD_TYPE_UNKNOWN,
        GAMEPAD_TYPE_NINTENDO_3DS,
        GAMEPAD_TYPE_NINTEDO_SWITCH_PRO,
        GAMEPAD_TYPE_JOYCON_LEFT,
        GAMEPAD_TYPE_JOYCON_RIGHT,
        GAMEPAD_TYPE_JOYCON_PAIR,
        GAMEPAD_TYPE_WII_U_GAMEPAD,
        GAMEPAD_TYPE_WII_REMOTE,
        GAMEPAD_TYPE_WII_REMOTE_NUNCHUCK,
        GAMEPAD_TYPE_WII_CLASSIC,
        GAMEPAD_TYPE_WII_PRO,
        GAMEPAD_TYPE_MAX_ENUM
    };

    /*
    ** device guid values
    ** generated from the following site:
    ** https://www.guidgenerator.com/online-guid-generator.aspx
    */
    std::string GetDeviceGUID(GamepadType type);

    bool GetConstant(const char* in, GamepadType& out);
    bool GetConstant(GamepadType in, const char*& out);
    std::vector<const char*> GetConstants(GamepadType);
} // namespace love::guid
