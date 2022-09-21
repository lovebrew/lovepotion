#include <utilities/wpad.hpp>

#include <utilities/bidirectionalmap.hpp>

using namespace love;

// clang-format off
constexpr auto wpadTypes = BidirectionalMap<>::Create(
    guid::GAMEPAD_TYPE_WII_REMOTE, WPAD_EXT_CORE | WPAD_EXT_MPLUS
);
// clang-format on

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
