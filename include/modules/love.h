/*
** modules/love.h
** @brief    : Handles all the modules
*/

#pragma once

class Love
{
    public:
        static int Initialize(lua_State * L);

        static void InitModules(lua_State * L);

        static void InitConstants(lua_State * L);

        static bool IsRunning();

        static int RaiseError(const char * format, ...);

        static int Quit(lua_State * L);

        static void Exit(lua_State * L);

    private:
        static inline bool quit = false;
        static inline bool error = false;

        static inline lua_State * state = nullptr;

        static int GetVersion(lua_State * L);

        static int Run(lua_State * L);

        static int NoGame(lua_State * L);
};
