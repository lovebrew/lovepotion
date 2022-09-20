#include <utilities/wpad.hpp>

using namespace love;

guid::GamepadType love::wpad::GetWPADType(KPADExtensionType extension)
{
    switch (extension)
    {
        case WPAD_EXT_CORE:
        case WPAD_EXT_MPLUS:
        default:
            return guid::GAMEPAD_TYPE_WII_REMOTE;
        case WPAD_EXT_CLASSIC:
        case WPAD_EXT_MPLUS_CLASSIC:
            return guid::GAMEPAD_TYPE_WII_CLASSIC;
        case WPAD_EXT_NUNCHUK:
        case WPAD_EXT_MPLUS_NUNCHUK:
            return guid::GAMEPAD_TYPE_WII_REMOTE_NUNCHUCK;
        case WPAD_EXT_PRO_CONTROLLER:
            return guid::GAMEPAD_TYPE_WII_PRO;
    }
}
