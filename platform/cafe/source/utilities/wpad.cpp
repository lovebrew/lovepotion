#include <utilities/wpad.hpp>

using namespace love;

guid::GamepadType love::wpad::GetWPADType(WPADExtensionType extension)
{
    guid::GamepadType type;

    switch (extension)
    {
        case WPAD_EXT_CORE:
        case WPAD_EXT_MPLUS:
        {
            type = guid::GAMEPAD_TYPE_WII_REMOTE;
            break;
        }
        case WPAD_EXT_CLASSIC:
        case WPAD_EXT_MPLUS_CLASSIC:
        {
            type = guid::GAMEPAD_TYPE_WII_CLASSIC;
            break;
        }
        case WPAD_EXT_NUNCHUK:
        case WPAD_EXT_MPLUS_NUNCHUK:
        {
            type = guid::GAMEPAD_TYPE_WII_REMOTE_NUNCHUCK;
            break;
        }
        case WPAD_EXT_PRO_CONTROLLER:
        {
            type = guid::GAMEPAD_TYPE_WII_PRO;
            break;
        }
    }

    return type;
}
