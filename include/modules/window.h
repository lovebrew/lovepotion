#pragma once

namespace Window
{
    void Initialize();

    SDL_Renderer * GetRenderer();

    //LÖVE functions

    int SetMode(lua_State * L);

    int GetFullscreenModes(lua_State * L);

    //End LÖVE functions

    int Register(lua_State * L);

    void Exit();
}