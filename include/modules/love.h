#pragma once

#define TITLE_TAKEOVER_ERROR "Please run Löve Potion under Atmosphère title takeover."

namespace Love
{
    int Initialize(lua_State * L);
    int Boot(lua_State * L);

    int GetVersion(lua_State * L);

    // Various Utility Functions

    int EnsureApplicationType(lua_State * L);

    void GetField(lua_State * L, const char * field);
    int Preload(lua_State * L, lua_CFunction func, const char * name);

    int InsistRegistry(lua_State * L, Registry registry);
    int GetRegistry(lua_State * L, Registry registry);

    // End Various Utility Functions

    typedef struct {
        const char * name;
        int (* reg)(lua_State * L);
    } Module;

    inline std::array<Love::Module, 15> modules = { nullptr };
};
