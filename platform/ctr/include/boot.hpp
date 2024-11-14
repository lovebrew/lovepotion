#pragma once

#include "common/luax.hpp"

#include <memory>

namespace love
{
    struct deleter
    {
        void operator()(void* pointer)
        {
            free(pointer);
        }
    };

    static constexpr int SOC_BUFFER_SIZE  = 0x100000;
    static constexpr int SOC_BUFFER_ALIGN = 0x1000;
    inline std::unique_ptr<uint32_t[], deleter> socBuffer;

    std::string getApplicationPath(const std::string& argv0);

    int preInit();

    bool mainLoop(lua_State* L, int argc, int* nres);

    void onExit();
} // namespace love
