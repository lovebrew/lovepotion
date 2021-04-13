#pragma once

#include "common/luax.h"
#include "objects/gamepad/gamepad.h"

namespace Wrap_Gamepad
{
    int GetAxes(lua_State* L);

    int GetAxis(lua_State* L);

    int GetButtonCount(lua_State* L);

    int GetGamepadAxis(lua_State* L);

    int GetID(lua_State* L);

    int GetName(lua_State* L);

    int GetVibration(lua_State* L);

    int IsConnected(lua_State* L);

    int IsDown(lua_State* L);

    int IsGamepad(lua_State* L);

    int IsGamepadDown(lua_State* L);

    int IsVibrationSupported(lua_State* L);

    int SetVibration(lua_State* L);

    love::Gamepad* CheckJoystick(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_Gamepad
