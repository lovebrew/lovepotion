#pragma once

#include "common/luax.hpp"

namespace love
{
    std::string getApplicationPath(const std::string& argv0 = "");

    int preInit();

    bool mainLoop(lua_State* L, int argc, int* nres);

    void onExit();
} // namespace love
