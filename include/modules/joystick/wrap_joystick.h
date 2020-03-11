#pragma once

#include "objects/gamepad/gamepad.h"
#include "modules/joystick/joystick.h"

namespace Wrap_Joystick
{
    int GetJoystickCount(lua_State * L);

    int GetJoysticks(lua_State * L);

    int Register(lua_State * L);
}
