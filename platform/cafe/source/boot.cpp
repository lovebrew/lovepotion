#include "boot.hpp"
#include "common/Console.hpp"
#include "common/service.hpp"

#include "driver/EventQueue.hpp"

#include <coreinit/bsp.h>
#include <coreinit/core.h>
#include <coreinit/exit.h>
#include <coreinit/filesystem.h>
#include <coreinit/foreground.h>

#include <nn/ac/ac_c.h>

#include <proc_ui/procui.h>

#include <padscore/kpad.h>
#include <vpad/input.h>

#include <sysapp/launch.h>

namespace love
{
    // clang-format off
    static constexpr std::array<const Service, 6> services =
    {{
        { "procUI", BIND(ProcUIInit, OSSavesDone_ReadyToRelease), &ProcUIShutdown },
        { "vpad",   BIND(VPADInit),                               &VPADShutdown   },
        { "kpad",   BIND(KPADInit),                               &KPADShutdown   },
        { "ac",     BIND(ACInitialize),                           &ACFinalize     },
        { "fs",     BIND(FSInit),                                 &FSShutdown     },
        { "bsp",    BIND(bspInitializeShimInterface),             []() { }        }
    }};
    // clang-format on

    uint32_t Console::mainCoreId = 0;
    bool Console::mainCoreIdSet  = false;

    int preInit()
    {
        for (const auto& service : services)
        {
            if (!service.init().success())
                return -1;
        }

        Console::setMainCoreId(OSGetCoreId());

        return 0;
    }

    static bool isRunning()
    {
        if (!Console::isMainCoreId(OSGetMainCoreId()))
        {
            ProcUISubProcessMessages(true);
            return true;
        }

        const auto status = ProcUIProcessMessages(true);

        switch (status)
        {
            case PROCUI_STATUS_IN_FOREGROUND:
                EventQueue::getInstance().sendFocus(true);
                break;
            case PROCUI_STATUS_RELEASE_FOREGROUND:
                EventQueue::getInstance().sendFocus(false);
                ProcUIDrawDoneRelease();
                break;
            case PROCUI_STATUS_EXITING:
                EventQueue::getInstance().sendQuit();
                return false;
            default:
                break;
        }

        return true;
    }

    static bool s_Shutdown = false;

    bool mainLoop(lua_State* L, int argc, int* nres)
    {
        if (!s_Shutdown)
        {
            const auto yielding = (luax_resume(L, argc, nres) == LUA_YIELD);

            if (!yielding)
            {
                SYSLaunchMenu();
                s_Shutdown = true;
            }
        }

        return isRunning();
    }

    void onExit()
    {
        for (auto it = services.rbegin(); it != services.rend(); ++it)
            it->exit();
    }
} // namespace love
