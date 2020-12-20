#include "common/runtime.h"
#include "modules/joystick/joystick.h"

using namespace love;

std::string Joystick::GetDeviceGUID(u32 index) const
{
    // if (index < 0 || index > MAX_GAMEPADS)
    //     return "";

    // std::string val = "Joycon";
    //     HidControllerType type;

    // HidControllerID hidID = CONTROLLER_P1_AUTO;
    // if (index != 0)
    //     hidID = static_cast<HidControllerID>(index);

    // type = hidGetControllerType(hidID);
    // std::string ret = "";

    // switch (type)
    // {
    //     case TYPE_PROCONTROLLER:
    //         ret = "Pro Controller";
    //         break;
    //     case TYPE_HANDHELD:
    //         ret = "Handheld";
    //         break;
    //     case TYPE_JOYCON_LEFT:
    //         ret = "Joycon Left";
    //         break;
    //     case TYPE_JOYCON_RIGHT:
    //         ret = "Joycon Right";
    //         break;
    //     default:
    //         break;
    // }

    // return ret += " (Player #" + std::to_string(index) + ")";
    return std::string("");
}

bool Joystick::Split(size_t id, const std::string & holdType)
{
    if (id < 0 || id > MAX_GAMEPADS)
        return false;

    Result success = 0;

    // success = hidSetNpadJoyAssignmentModeSingleByDefault(static_cast<HidControllerID>(id));

    /* TO-DO: holdType horizontal */

    return R_SUCCEEDED(success);
}

bool Joystick::Merge(const std::array<size_t, 2> & gamepads)
{
    Result success = 0;

    // for (auto id : gamepads)
    //     success = hidSetNpadJoyAssignmentModeDual(static_cast<HidControllerID>(id));

    // if (R_SUCCEEDED(success))
    //     hidMergeSingleJoyAsDualJoy(static_cast<HidControllerID>(gamepads[0]), static_cast<HidControllerID>(gamepads[1]));

    return R_SUCCEEDED(success);
}
