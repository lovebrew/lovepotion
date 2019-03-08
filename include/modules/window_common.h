#pragma once

namespace Window
{
    void Initialize();

    int SetMode(lua_State * L);

    int GetFullscreenModes(lua_State * L);

    int ShowMessageBox(lua_State * L);

    int Register(lua_State * L);

    void Exit();
}