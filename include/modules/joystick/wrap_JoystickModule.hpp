#pragma once

#include "common/luax.hpp"
#include "modules/joystick/JoystickModule.hpp"

namespace Wrap_JoystickModule
{
    int getJoysticks(lua_State* L);

    int getIndex(lua_State* L);

    int getJoystickCount(lua_State* L);

    int setGamepadMapping(lua_State* L);

    int loadGamepadMappings(lua_State* L);

    int saveGamepadMappings(lua_State* L);

    int getGamepadMappingString(lua_State* L);

    int open(lua_State* L);
} // namespace Wrap_JoystickModule
