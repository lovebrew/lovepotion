#include "utility/guid.hpp"

namespace love
{
    /*
    ** Device GUID values are generated from the following site:
    ** https://www.guidgenerator.com/online-guid-generator.aspx
    */

    /*
    ** Buttons: D-Pad (4) ABXY (4) L/R (2) Start/Select (2) Stick L/R (2, if available)
    ** Axes: Left Stick X/Y, Right Stick X/Y (if available), ZL/ZR Trigger (if available)
    */

    // clang-format off
    static constexpr GamepadInfo gamepadInfo[] =
    {   /* buttonCount, axisCount, hatCount, guid, hasZL, hasZR, hasGyro, hasAccel */
        {  0,   0,  0, "{00000000-0000-0000-0000-000000000000}", false, false, false, false }, // GAMEPAD_TYPE_UNKNOWN
        { 12,   2,  0, "{B58A259A-13AA-46E0-BDCB-31898EDAB24E}", false, false, true,  true  }, // GAMEPAD_TYPE_NINTENDO_3DS
        { 12,   2,  0, "{4135DAAA-9F38-4709-9D5E-A010DF61C2E7}", false, false, true,  true  }, // GAMEPAD_TYPE_NINTENDO_3DS_XL
        { 12,   6,  0, "{7BC9702D-7D81-4EBB-AD4F-8C94076588D5}", true,  true,  true,  true  }, // GAMEPAD_TYPE_NEW_NINTENDO_3DS
        { 12,   6,  0, "{2418ED2A-7918-4D2C-B390-6127D5FEB977}", true,  true,  true,  true  }, // GAMEPAD_TYPE_NEW_NINTENDO_3DS_XL
        { 12,   2,  0, "{3846FA8C-25AB-44CB-91FD-D58FB1F572CC}", false, false, true,  true  }, // GAMEPAD_TYPE_NINTENDO_2DS
        { 12,   6,  0, "{560CDDB3-0594-4A52-A3D5-617060D2001B}", true,  true,  true,  true  }, // GAMEPAD_TYPE_NEW_NINTENDO_2DS_XL
        { 10,   0,  0, "{02DC4D7B-2480-4678-BB06-D9AEDC3DE29B}", false, false, false, true  }, // GAMEPAD_TYPE_NINTENDO_WII_REMOTE
        { 12,   2,  0, "{C0E2DDE5-25DF-4F7D-AEA6-4F25DE2FC385}", false, false, false, true  }, // GAMEPAD_TYPE_NINTENDO_WII_REMOTE_NUNCHUK
        { 12,   6,  0, "{B4F6A311-8228-477D-857B-B875D891C46D}", true,  true,  false, false }, // GAMEPAD_TYPE_NINTENDO_WII_CLASSIC
        { 14,   6,  0, "{62998927-C43D-41F5-B6B1-D22CBF031D91}", true,  true,  true,  true  }, // GAMEPAD_TYPE_WII_U_GAMEPAD
        { 14,   6,  0, "{36895D3B-A724-4F46-994C-64BCE736EBCB}", true,  true,  false, false }, // GAMEPAD_TYPE_WII_U_PRO
        { 14,   6,  0, "{6EBE242C-820F-46E1-9A66-DC8200686D51}", true,  true,  true,  true  }, // GAMEPAD_TYPE_NINTENDO_SWITCH_HANDHELD
        { 14,   6,  0, "{42ECF5C5-AFA5-4EDE-B1A2-4E9C2287559A}", true,  false, true,  true  }, // GAMEPAD_TYPE_NINTENDO_SWITCH_PRO
        {  7,   3,  0, "{660EBC7E-3953-4B74-8406-AD5992FCC5C7}", true,  false, true,  true  }, // GAMEPAD_TYPE_JOYCON_LEFT
        {  7,   3,  0, "{AD770831-A7E4-41A8-8DD0-FD48323E0043}", false, true,  true,  true  }, // GAMEPAD_TYPE_JOYCON_RIGHT
        { 14,   6,  0, "{701B198B-9AD9-4730-8EEB-EBECF707B9DF}", true,  true,  true,  true  }, // GAMEPAD_TYPE_JOYCON_PAIR
    };
    static_assert(sizeof(gamepadInfo) / sizeof(GamepadInfo) == GAMEPAD_TYPE_MAX_ENUM, "Update the gamepadInfo array when adding or removing a GamepadInfo");
    // clang-format on

    int getGamepadButtonCount(GamepadType type)
    {
        return gamepadInfo[type].buttonCount;
    }

    int getGamepadAxisCount(GamepadType type)
    {
        return gamepadInfo[type].axisCount;
    }

    int getGamepadHatCount(GamepadType type)
    {
        return gamepadInfo[type].hatCount;
    }

    const char* getGamepadGUID(GamepadType type)
    {
        return gamepadInfo[type].guid;
    }

    bool getGamepadHasZL(GamepadType type)
    {
        return gamepadInfo[type].hasZL;
    }

    bool getGamepadHasZR(GamepadType type)
    {
        return gamepadInfo[type].hasZR;
    }

    bool getGamepadHasGyroscope(GamepadType type)
    {
        return gamepadInfo[type].hasGyro;
    }

    bool getGamepadHasAccelerometer(GamepadType type)
    {
        return gamepadInfo[type].hasAccel;
    }

    bool getGamepadDeviceInfo(GamepadType type, int& vendorId, int& productId, int& productVersion)
    {
        vendorId       = 0x057E;
        productId      = 0x0000;
        productVersion = 0x0001;

        switch (type)
        {
            case GAMEPAD_TYPE_NINTENDO_SWITCH_HANDHELD:
                productId = 0x2000;
                break;
            case GAMEPAD_TYPE_NINTENDO_SWITCH_PRO:
                productId = 0x2009;
                break;
            case GAMEPAD_TYPE_JOYCON_LEFT:
                productId = 0x2006;
                break;
            case GAMEPAD_TYPE_JOYCON_RIGHT:
                productId = 0x2007;
                break;
            default:
                return false;
        }

        return true;
    }
} // namespace love
