#pragma once

#include "common/luax.hpp"
#include "modules/joystick/Joystick.tcc"

namespace love
{
    JoystickBase* luax_checkjoystick(lua_State* L, int idx);

    int open_joystick(lua_State* L);
} // namespace love

namespace Wrap_Joystick
{
    int isConected(lua_State* L);

    int getName(lua_State* L);

    int getID(lua_State* L);

    int getGUID(lua_State* L);

    int getDeviceInfo(lua_State* L);

    int getJoystickType(lua_State* L);

    int getAxisCount(lua_State* L);

    int getButtonCount(lua_State* L);

    int getHatCount(lua_State* L);

    int getAxis(lua_State* L);

    int getAxes(lua_State* L);

    int getHat(lua_State* L);

    int isDown(lua_State* L);

    int setPlayerIndex(lua_State* L);

    int getPlayerIndex(lua_State* L);

    int isGamepad(lua_State* L);

    int getGamepadType(lua_State* L);

    int getGamepadAxis(lua_State* L);

    int isGamepadDown(lua_State* L);

    int getGamepadMapping(lua_State* L);

    int getGamepadMappingString(lua_State* L);

    int isVibrationSupported(lua_State* L);

    int setVibration(lua_State* L);

    int getVibration(lua_State* L);

    int hasSensor(lua_State* L);

    int isSensorEnabled(lua_State* L);

    int setSensorEnabled(lua_State* L);

    int getSensorData(lua_State* L);

    int getPosition(lua_State* L);

    int getAngle(lua_State* L);
} // namespace Wrap_Joystick
