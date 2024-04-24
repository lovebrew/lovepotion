#include "boot.hpp"
#include "common/Console.hpp"
#include "common/service.hpp"

#include "driver/EventQueue.hpp"

#include <coreinit/bsp.h>
#include <coreinit/core.h>
#include <coreinit/dynload.h>
#include <coreinit/exit.h>
#include <coreinit/filesystem.h>
#include <coreinit/foreground.h>

#include <nn/ac/ac_c.h>

#include <proc_ui/procui.h>

#include <padscore/kpad.h>
#include <vpad/input.h>

#include <sysapp/launch.h>

#include "utility/logfile.hpp"

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

    std::string getApplicationPath(const std::string& argv0)
    {
        if (argv0 == "embedded boot.lua")
            return "fs:/vol/external01/lovepotion.wuhb";

        OSDynLoad_Module module;
        const auto type  = OS_DYNLOAD_EXPORT_FUNC;
        const char* name = "RL_GetPathOfRunningExecutable";

        if (OSDynLoad_Acquire("homebrew_rpx_loader", &module) == OS_DYNLOAD_OK)
        {
            char path[256];

            bool (*RL_GetPathOfRunningExecutable)(char*, uint32_t);
            auto** function = reinterpret_cast<void**>(&RL_GetPathOfRunningExecutable);

            if (OSDynLoad_FindExport(module, type, name, function) == OS_DYNLOAD_OK)
            {
                if (RL_GetPathOfRunningExecutable(path, sizeof(path)) == 0)
                    return path;
            }
        }

        return std::string {};
    }

    int preInit()
    {
        /* we aren't running Aroma */
        if (getApplicationPath().empty())
            return -1;

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
