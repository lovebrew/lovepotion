#pragma once

#pragma once

#include <modules/joystickmodule_ext.hpp>
#include <objects/joystick_ext.hpp>

#include <common/luax.hpp>

namespace Wrap_JoystickModule
{
    int GetJoysticks(lua_State* L);

    int GetIndex(lua_State* L);

    int GetJoystickCount(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_JoystickModule
