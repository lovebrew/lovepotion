#include "common/error.hpp"
#include "common/service.hpp"

#include "boot.hpp"

#include <array>
#include <format>
#include <string>

#include <switch.h>

namespace love
{

    // clang-format off
    static constexpr std::array<const Service, 8> services =
    {{
        { "bsd:u",    BIND(socketInitializeDefault),                                  &socketExit  },
        { "pl:u",     BIND(plInitialize,             PlServiceType_User),             &plExit      },
        { "acc:a",    BIND(accountInitialize,        AccountServiceType_Application), &accountExit },
        { "set",      BIND(setInitialize),                                            &setExit     },
        { "set:sys",  BIND(setsysInitialize),                                         &setsysExit  },
        { "psm",      BIND(psmInitialize),                                            &psmExit     },
        { "friend:u", BIND(friendsInitialize,        FriendsServiceType_User),        &friendsExit },
        { "nifm:u",   BIND(nifmInitialize,           NifmServiceType_User),           &nifmExit    }
    }};
    // clang-format on

    static bool isUnderTitleTakeover()
    {
        const auto appletType = appletGetAppletType();
        return appletType == AppletType_Application || appletType == AppletType_SystemApplication;
    }

    template<typename... Args>
    static int displayError(const char* format, int32_t result, Args&&... args)
    {
        std::string message = std::vformat(format, std::make_format_args(args...));

        ErrorApplicationConfig config {};
        errorApplicationCreate(&config, message.c_str(), nullptr);
        errorApplicationSetNumber(&config, result);
        errorApplicationShow(&config);

        return -1;
    }

    std::string getApplicationPath(const std::string& argv0)
    {
        if (argv0 == "embedded boot.lua")
            return "sdmc:/lovepotion.nro";

        return argv0;
    }

    int preInit()
    {
        if (!isUnderTitleTakeover())
            return displayError(E_TITLE_TAKEOVER, 1043);

        for (auto& service : services)
        {
            if (auto result = service.init(); !result)
                return displayError("Failed to initialize {:s}.", result.get(), service.name);
        }

        padConfigureInput(8, HidNpadStyleSet_NpadStandard);
        hidInitializeTouchScreen();

        romfsInit();

        return 0;
    }

    bool mainLoop(lua_State* L, int argc, int* nres)
    {
        return love::luax_resume(L, argc, nres) == LUA_YIELD;
    }

    void onExit()
    {
        romfsExit();

        for (auto it = services.rbegin(); it != services.rend(); ++it)
            it->exit();
    }
} // namespace love
