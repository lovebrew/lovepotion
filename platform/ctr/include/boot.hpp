#pragma once

#include "common/luax.hpp"

namespace love
{
    static constexpr int SOC_BUFFER_SIZE  = 0x100000;
    static constexpr int SOC_BUFFER_ALIGN = 0x1000;
    inline uint32_t* socBuffer            = nullptr;

    void preInit();

    bool mainLoop(lua_State* L, int argc, int* nres);

    void onExit();
} // namespace love
