#include "common/runtime.h"
#include "modules/joystick/joystick.h"

using namespace love;

std::string Joystick::GetDeviceGUID(u32 index) const
{
    if (index < 0 || index > MAX_GAMEPADS)
        return "";

    return "Nintendo 3DS";
}

bool Joystick::Split(size_t id, const std::string & holdType)
{
    return false;
}

bool Joystick::Merge(const std::array<size_t, 2> & gamepads)
{
    return false;
}
