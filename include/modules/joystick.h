#pragma once

namespace Joystick
{
    void Initialize();

    int GetJoysticks(lua_State * L);

    void Exit();

    int Register(lua_State * L);
};