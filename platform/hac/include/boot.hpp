#pragma once

#include "common/luax.hpp"

namespace love
{
    int preInit();

    bool mainLoop(lua_State* L, int argc, int* nres);

    void onExit();
} // namespace love
