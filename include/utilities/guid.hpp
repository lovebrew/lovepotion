#pragma once

#include <objects/joystick/joystick.tcc>

#include <utilities/bidirectionalmap/smallvector.hpp>

#include <vector>

namespace love::guid
{
    enum GamepadType
    {
        GAMEPAD_TYPE_UNKNOWN,
        GAMEPAD_TYPE_NINTENDO_3DS,
        GAMEPAD_TYPE_NEW_NINTENDO_3DS,
        GAMEPAD_TYPE_NINTENDO_SWITCH_HANDHELD,
        GAMEPAD_TYPE_NINTENDO_SWITCH_PRO,
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

    struct GamepadInfo
    {
        int buttonCount;
        int axisCount;
        int hatCount;
        const char* name;
        const char* guid;
        bool hasZL;
        bool hasZR;
    };

    struct DeviceInfo
    {
        int vendorId;
        int productId;
        int productVersion;
    };

    int GetGamepadButtonCount(GamepadType type);

    int GetGamepadAxisCount(GamepadType type);

    int GetGamepadHatCount(GamepadType type);

    const char* GetGamepadGUID(GamepadType type);

    const char* GetGamepadName(GamepadType type);

    bool GetGamepadHasZL(GamepadType type);

    bool GetGamepadHasZR(GamepadType type);

    bool GetDeviceInfo(GamepadType type, DeviceInfo& info);

    bool GetConstant(const char* in, GamepadType& out);
    bool GetConstant(GamepadType in, const char*& out);
    SmallTrivialVector<const char*, 13> GetConstants(GamepadType);
} // namespace love::guid
