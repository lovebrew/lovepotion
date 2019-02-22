/*
** modules/love.h
** @brief    : Handles all the modules
*/

#pragma once

// Forward declare lua_State.
// Official LÖVE does this!
struct lua_State;
extern bool LUA_ERROR;

namespace Love
{
    int Initialize(lua_State * L);

    void InitModules(lua_State * L);
    
    void InitConstants(lua_State * L);

    int GetVersion(lua_State * L);

    int EnableConsole(lua_State * L);

    bool IsRunning();

    int RaiseError(const char * format, ...);

    int Run(lua_State * L);

    int NoGame(lua_State * L);

    void Exit(lua_State * L);
}