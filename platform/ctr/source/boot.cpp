#include "boot.hpp"

#include <3ds.h>

#include <stdlib.h>

namespace love
{
    void preInit()
    {
        socBuffer = (uint32_t*)aligned_alloc(SOC_BUFFER_ALIGN, SOC_BUFFER_SIZE);
        socInit(socBuffer, SOC_BUFFER_SIZE);
    }

    bool mainLoop(lua_State* L, int argc, int* nres)
    {
        return ((love::luax_resume(L, argc, nres) == LUA_YIELD) && aptMainLoop());
    }

    void onExit()
    {
        socExit();

        if (socBuffer)
            free(socBuffer);
    }
} // namespace love
