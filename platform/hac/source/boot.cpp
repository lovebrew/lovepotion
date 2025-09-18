#include "common/error.hpp"
#include "common/service.hpp"

#include "boot.hpp"
#include "modules/love/love.hpp"

#include <array>
#include <format>
#include <string>

#include <switch.h>

namespace love
{
    namespace platform
    {
        // clang-format off
        static constexpr std::array<const Service, 9> services =
        {{
            { "bsd:u",    BIND(socketInitializeDefault),                                  &socketExit                 },
            { "pl:u",     BIND(plInitialize,             PlServiceType_User),             &plExit                     },
            { "acc:a",    BIND(accountInitialize,        AccountServiceType_Application), &accountExit                },
            { "set",      BIND(setInitialize),                                            &setExit                    },
            { "set:sys",  BIND(setsysInitialize),                                         &setsysExit                 },
            { "psm",      BIND(psmInitialize),                                            &psmExit                    },
            { "friend:u", BIND(friendsInitialize,        FriendsServiceType_User),        &friendsExit                },
            { "nifm:u",   BIND(nifmInitialize,           NifmServiceType_User),           &nifmExit                   },
            { "romfs",    BIND(romfsInit),                                                []() { (void)romfsExit(); } }
        }};
        // clang-format on

        static bool isUnderTitleTakeover()
        {
            const auto appletType = appletGetAppletType();
            return appletType == AppletType_Application || appletType == AppletType_SystemApplication;
        }

        void errorHandler(const std::string& message)
        {
            consoleInit(NULL);

            std::printf("Error:\n\n%s\n", message.c_str());
            std::printf("Press + to quit.\n");

            PadState pad {};
            padInitializeDefault(&pad);

            while (appletMainLoop())
            {
                padUpdate(&pad);
                if (padGetButtonsDown(&pad) & HidNpadButton_Plus)
                    break;

                consoleUpdate(NULL);
            }

            consoleExit(NULL);
        }

        std::string getApplicationPath(const std::string& argv0)
        {
            if (argv0 == "embedded boot.lua")
                return "sdmc:/lovepotion.nro";

            return argv0;
        }

        int initialize()
        {
            padConfigureInput(8, HidNpadStyleSet_NpadStandard);
            hidInitializeTouchScreen();

            if (!isUnderTitleTakeover())
            {
                errorHandler(E_TITLE_TAKEOVER);
                return EXIT_FAILURE;
            }

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
            return love::luax_resume(L, argc, nres) == LUA_YIELD;
        }

        void shutdown()
        {
            g_debugSocket.restore();
            for (auto it = services.rbegin(); it != services.rend(); ++it)
                it->exit();
        }
    } // namespace platform
} // namespace love
