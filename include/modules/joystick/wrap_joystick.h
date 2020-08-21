#pragma once

#include "objects/gamepad/gamepad.h"
#include "modules/joystick/joystick.h"

namespace Wrap_Joystick
{
    int GetJoystickCount(lua_State * L);

    int GetJoysticks(lua_State * L);

    int Split(lua_State * L);

    int Merge(lua_State * L);

    love::Gamepad * CheckGamepad(lua_State * L, int index);

    int Register(lua_State * L);
}
