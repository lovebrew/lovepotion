#pragma once

namespace love
{
    struct GamepadInfo
    {
        int buttonCount;
        int axisCount;
        int hatCount;
        const char* guid;
        bool hasZL;
        bool hasZR;
        bool hasGyro;
        bool hasAccel;
    };

    enum GamepadType
    {
        GAMEPAD_TYPE_UNKNOWN,
        GAMEPAD_TYPE_NINTENDO_3DS,
        GAMEPAD_TYPE_NINTENDO_3DS_XL,
        GAMEPAD_TYPE_NEW_NINTENDO_3DS,
        GAMEPAD_TYPE_NEW_NINTENDO_3DS_XL,
        GAMEPAD_TYPE_NINTENDO_2DS,
        GAMEPAD_TYPE_NEW_NINTENDO_2DS_XL,
        GAMEPAD_TYPE_NINTENDO_WII_REMOTE,
        GAMEPAD_TYPE_NINTENDO_WII_REMOTE_NUNCHUK,
        GAMEPAD_TYPE_NINTENDO_WII_CLASSIC,
        GAMEPAD_TYPE_NINTENDO_WII_U_GAMEPAD,
        GAMEPAD_TYPE_NINTENDO_WII_U_PRO,
        GAMEPAD_TYPE_NINTENDO_SWITCH_HANDHELD,
        GAMEPAD_TYPE_NINTENDO_SWITCH_PRO,
        GAMEPAD_TYPE_JOYCON_LEFT,
        GAMEPAD_TYPE_JOYCON_RIGHT,
        GAMEPAD_TYPE_JOYCON_PAIR,
        GAMEPAD_TYPE_MAX_ENUM
    };

    int getGamepadButtonCount(GamepadType type);

    int getGamepadAxisCount(GamepadType type);

    int getGamepadHatCount(GamepadType type);

    const char* getGamepadGUID(GamepadType type);

    bool getGamepadHasZL(GamepadType type);

    bool getGamepadHasZR(GamepadType type);

    bool getGamepadHasGyroscope(GamepadType type);

    bool getGamepadHasAccelerometer(GamepadType type);

    bool getGamepadDeviceInfo(GamepadType type, int& vendorID, int& productID, int& productVersion);
} // namespace love
