#include "boot.hpp"

#include <3ds.h>

#include <stdlib.h>

namespace love
{
    // clang-format off
    static constexpr std::array services =
    {
        std::pair { &mcuHwcInit, &mcuHwcExit },
        std::pair { &ptmuInit,   &ptmuExit   },
        std::pair { &acInit,     &acExit     },
        std::pair { &cfguInit,   &cfguExit   },
        std::pair { &frdInit,    &frdExit    }
    };
    // clang-format on

    void preInit()
    {
        osSetSpeedupEnable(true);

        romfsInit();

        socBuffer = (uint32_t*)aligned_alloc(SOC_BUFFER_ALIGN, SOC_BUFFER_SIZE);
        socInit(socBuffer, SOC_BUFFER_SIZE);

        for (auto& service : services)
            service.first();
    }

    bool mainLoop(lua_State* L, int argc, int* nres)
    {
        return ((love::luax_resume(L, argc, nres) == LUA_YIELD) && aptMainLoop());
    }

    void onExit()
    {
        for (auto it = services.rbegin(); it != services.rend(); ++it)
            it->second();

        if (socBuffer)
            free(socBuffer);

        romfsExit();
    }
} // namespace love
