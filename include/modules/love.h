#pragma once

namespace Love
{
    int Initialize(lua_State* L);

    int Boot(lua_State* L);

    int NoGame(lua_State* L);

    int GetVersion(lua_State* L);

    int EnableAccelerometerAsJoystick(lua_State* L);

    int IsVersionCompatible(lua_State* L);

    /* init nxlink or something */
    int _OpenConsole(lua_State* L);

    /* -------- */

    int Preload(lua_State* L, lua_CFunction func, const char* name);
}; // namespace Love
