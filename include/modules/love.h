#pragma once

#define TITLE_TAKEOVER_ERROR "Please run Löve Potion under Atmosphère title takeover."

#if defined(_3DS)
    #define MAX_MODULES 16
#else
    #define MAX_MODULES 17
#endif

namespace Love
{
    int Initialize(lua_State * L);
    int Boot(lua_State * L);

    int GetVersion(lua_State * L);

    int EnableAccelerometerAsJoystick(lua_State * L);

    // Various Utility Functions

    int _EnsureApplicationType(lua_State * L);

    void GetField(lua_State * L, const char * field);
    int Preload(lua_State * L, lua_CFunction func, const char * name);

    int InsistRegistry(lua_State * L, Registry registry);
    int GetRegistry(lua_State * L, Registry registry);

    /* init nxlink or something */
    int _OpenConsole(lua_State * L);

    // End Various Utility Functions

    typedef struct {
        const char * name;
        int (* reg)(lua_State * L);
    } Module;

    inline std::array<Love::Module, MAX_MODULES> modules = { nullptr };

    inline int debugSockfd = -1;
};
