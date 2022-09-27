#include <utilities/wpad.hpp>

#include <utilities/bidirectionalmap/bidirectionalmultimap.hpp>

using namespace love;

// clang-format off
constexpr BidirectionalMultiMap wpadTypes = {
    WPAD_EXT_CORE, guid::GAMEPAD_TYPE_WII_REMOTE,
    WPAD_EXT_MPLUS, guid::GAMEPAD_TYPE_WII_REMOTE,
    WPAD_EXT_CLASSIC, guid::GAMEPAD_TYPE_WII_CLASSIC,
    WPAD_EXT_MPLUS_CLASSIC, guid::GAMEPAD_TYPE_WII_CLASSIC,
    WPAD_EXT_NUNCHUK, guid::GAMEPAD_TYPE_WII_REMOTE_NUNCHUCK,
    WPAD_EXT_MPLUS_NUNCHUK, guid::GAMEPAD_TYPE_WII_REMOTE_NUNCHUCK,
    WPAD_EXT_PRO_CONTROLLER, guid::GAMEPAD_TYPE_WII_PRO
};
// clang-format on

guid::GamepadType love::wpad::GetWPADType(KPADExtensionType extension)
{
    guid::GamepadType ret;
    if (wpadTypes.FindFirst(extension, ret))
    {
        return ret;
    }
    // Return unknown gamepad type for unknown extension, seems reasonable to me
    return guid::GamepadType::GAMEPAD_TYPE_UNKNOWN;
}
