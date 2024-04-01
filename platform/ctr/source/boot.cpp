#include "common/service.hpp"

#include "boot.hpp"

#include <3ds.h>

#include <stdlib.h>

#include <format>
#include <string>

namespace love
{
    std::unique_ptr<uint32_t[], unique_deleter> socBuffer;

    // clang-format off
    static constexpr std::array<const Service, 8> services =
    {{
        /* I don't know why socExit is a Result return, we don't care about the exit value */
        { "soc:u",   BIND(socInit, socBuffer.get(), SOC_BUFFER_SIZE), []() { socExit(); }    },
        { "mcu:Hwc", BIND(mcuHwcInit),                                &mcuHwcExit            },
        { "ptm:u",   BIND(ptmuInit),                                  &ptmuExit              },
        { "ac:u",    BIND(acInit),                                    &acExit                },
        { "cfg:u",   BIND(cfguInit),                                  &cfguExit              },
        { "frd:u",   BIND(frdInit),                                   &frdExit               },
        { "ir:rst",  BIND(irrstInit),                                 &irrstExit             },
        { "gsp:Gpu", BIND(gfxInitDefault),                            &gfxExit               }
    }};
    // clang-format on

    template<typename... Args>
    static int displayError(const char* format, int32_t result, Args&&... args)
    {
        std::string message = std::vformat(format, std::make_format_args(args...));

        errorConf config {};
        errorInit(&config, ERROR_TEXT_WORD_WRAP, CFG_LANGUAGE_EN);
        errorCode(&config, result);
        errorText(&config, message.c_str());
        errorDisp(&config);

        return -1;
    }

    int preInit()
    {
        osSetSpeedupEnable(true);

        socBuffer.reset((uint32_t*)aligned_alloc(SOC_BUFFER_ALIGN, SOC_BUFFER_SIZE));

        for (auto& service : services)
        {
            if (auto result = service.init(); !result)
                return displayError("Failed to initialize {:s}.", result.get(), service.name);
        }

        romfsInit();

        return 0;
    }

    bool mainLoop(lua_State* L, int argc, int* nres)
    {
        return ((love::luax_resume(L, argc, nres) == LUA_YIELD) && aptMainLoop());
    }

    void onExit()
    {
        romfsExit();

        for (auto it = services.rbegin(); it != services.rend(); ++it)
            it->exit();
    }
} // namespace love