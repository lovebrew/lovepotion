#include "boot.hpp"

#include <switch.h>

namespace love
{
    void preInit()
    {
        socketInitializeDefault();
    }

    bool mainLoop(lua_State* L, int argc, int* nres)
    {
        return love::luax_resume(L, argc, nres) == LUA_YIELD;
    }

    void onExit()
    {
        socketExit();
    }
} // namespace love
