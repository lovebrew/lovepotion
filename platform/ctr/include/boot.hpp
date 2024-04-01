#pragma once

#include "common/luax.hpp"

#include <memory>

namespace love
{
    struct unique_deleter
    {
        void operator()(void* ptr) const
        {
            free(ptr);
        }
    };

    static constexpr int SOC_BUFFER_SIZE  = 0x100000;
    static constexpr int SOC_BUFFER_ALIGN = 0x1000;
    extern std::unique_ptr<uint32_t[], unique_deleter> socBuffer;

    int preInit();

    bool mainLoop(lua_State* L, int argc, int* nres);

    void onExit();
} // namespace love
