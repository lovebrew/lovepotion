#include "common/service.hpp"

#include "boot.hpp"

#include <3ds.h>

#include <malloc.h>
#include <stdlib.h>

#include <format>
#include <string>

namespace love
{
    namespace platform
    {
        static constexpr int SOC_BUFFER_SIZE  = 0x100000;
        static constexpr int SOC_BUFFER_ALIGN = 0x1000;
        static uint32_t* SOC_BUFFER           = nullptr;

        static Result socuInit()
        {
            if (!(SOC_BUFFER = (uint32_t*)memalign(SOC_BUFFER_ALIGN, SOC_BUFFER_SIZE)))
                return MAKERESULT(RL_PERMANENT, RS_OUTOFRESOURCE, RM_APPLICATION, RD_NO_DATA);

            return socInit(SOC_BUFFER, SOC_BUFFER_SIZE);
        }

        static void socuExit()
        {
            socExit();

            if (SOC_BUFFER != nullptr)
                std::free(SOC_BUFFER);

            SOC_BUFFER = nullptr;
        }

        // clang-format off
        static constexpr std::array<const Service, 8> services =
        {{
            { "ac:u",    BIND(acInit),                                     &acExit               },
            { "soc:u",   BIND(socuInit),                                   &socuExit             },
            { "mcu:Hwc", BIND(mcuHwcInit),                                 &mcuHwcExit           },
            { "ptm:u",   BIND(ptmuInit),                                   &ptmuExit             },
            { "cfg:u",   BIND(cfguInit),                                   &cfguExit             },
            { "frd:a",   BIND(frdInit, false),                             &frdExit              },
            { "ir:rst",  BIND(irrstInit),                                  &irrstExit            },
            { "romfs",   BIND(romfsInit),                                  []() { romfsExit(); } }
        }};
        // clang-format on

        void errorHandler(const std::string& message)
        {
            gfxInitDefault();
            consoleInit(GFX_TOP, nullptr);

            std::printf("Error:\n\n%s\n", message.c_str());
            std::printf("Press START to quit.\n");

            while (aptMainLoop())
            {
                hidScanInput();
                const auto pressed = hidKeysDown();

                if (pressed & KEY_START)
                    break;

                gspWaitForVBlank();
                gfxSwapBuffers();
            }

            gfxExit();
        }

        std::string getApplicationPath(const std::string& argv0)
        {
            if (argv0 == "embedded boot.lua")
                return "sdmc:/lovepotion.3dsx";

            return argv0;
        }

        int initialize()
        {
            std::string buffer;
            for (auto it = services.begin(); it != services.end(); ++it)
            {
                if (auto result = it->init(); !result)
                    buffer.append(std::format(E_FAILED_TO_INIT, it->name, result.value()));
            }

            if (!buffer.empty())
            {
                errorHandler(buffer);
                return EXIT_FAILURE;
            }

            return 0;
        }

        bool run(lua_State* L, int argc, int* nres)
        {
            return ((love::luax_resume(L, argc, nres) == LUA_YIELD) && aptMainLoop());
        }

        void shutdown()
        {
            for (auto it = services.rbegin(); it != services.rend(); ++it)
                it->exit();
        }
    } // namespace platform
} // namespace love
