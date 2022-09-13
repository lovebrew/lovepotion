#pragma once

#include <common/console.hpp>

namespace love
{
    enum DoneAction
    {
        DONE_QUIT,
        DONE_RESTART
    };

    template<Console::Platform T = Console::ALL>
    void PreInit()
    {}

    int Initialize(lua_State* L);

    template<Console::Platform T = Console::ALL>
    void OnExit()
    {}

    int LoadArgs(lua_State* L);

    int LoadCallbacks(lua_State* L);

    int Boot(lua_State* L);

    int NoGame(lua_State* L);

    template<Console::Platform T = Console::ALL>
    bool IsRunningAppletMode()
    {
        return false;
    }

    template<Console::Platform T = Console::ALL>
    int Quit(lua_State* L)
    {
        return 0;
    }

    template<Console::Platform T = Console::ALL>
    int Restart(lua_State* L)
    {
        return 0;
    }

    template<Console::Platform T>
    bool MainLoop(lua_State* L, int numArgs);

    int GetVersion(lua_State* L);

    int IsVersionCompatible(lua_State* L);
} // namespace love
