#pragma once

#include <modules/joystickmodule_ext.hpp>
#include <objects/joystick_ext.hpp>

#include <common/luax.hpp>

namespace Wrap_Joystick
{
    love::Joystick<love::Console::Which>* CheckJoystick(lua_State* L, int index);

    int IsConnected(lua_State* L);

    int GetName(lua_State* L);

    int GetID(lua_State* L);

    int GetIndex(lua_State* L);

    int GetGUID(lua_State* L);

    int GetDeviceInfo(lua_State* L);

    int GetAxisCount(lua_State* L);

    int GetButtonCount(lua_State* L);

    int GetHatCount(lua_State* L);

    int GetAxis(lua_State* L);

    int GetAxes(lua_State* L);

    int GetHat(lua_State* L);

    int IsDown(lua_State* L);

    int SetPlayerIndex(lua_State* L);

    int GetPlayerIndex(lua_State* L);

    int IsGamepad(lua_State* L);

    int GetGamepadType(lua_State* L);

    int GetGamepadAxis(lua_State* L);

    int IsGamepadDown(lua_State* L);

    int IsVibrationSupported(lua_State* L);

    int SetVibration(lua_State* L);

    int GetVibration(lua_State* L);

    int Split(lua_State* L);

    int Join(lua_State* L);

    int Register(lua_State* L);

    extern std::span<const luaL_Reg> extension;
} // namespace Wrap_Joystick
