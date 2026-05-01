#include "boot.hpp"
#include "common/Console.hpp"
#include "common/service.hpp"
#include "fatal.hpp"

#include "driver/EventQueue.hpp"

#include <coreinit/bsp.h>
#include <coreinit/core.h>
#include <coreinit/dynload.h>
#include <coreinit/exit.h>
#include <coreinit/filesystem.h>
#include <coreinit/foreground.h>

#include <coreinit/thread.h>
#include <coreinit/time.h>

#include <vpad/input.h>
#include <whb/log.h>
#include <whb/log_console.h>
#include <whb/proc.h>

#include <nn/ac/ac_c.h>

#include <proc_ui/procui.h>

#include <padscore/kpad.h>
#include <vpad/input.h>

#include <sysapp/launch.h>

namespace love
{
    uint32_t Console::mainCoreId = 0;
    bool Console::mainCoreIdSet  = false;

    namespace platform
    {
        // clang-format off
        static constexpr std::array<const Service,5> services =
        {{
            { "procUI", BIND(ProcUIInit, OSSavesDone_ReadyToRelease), &ProcUIShutdown },
            { "vpad",   BIND(VPADInit),                               &VPADShutdown   },
            { "kpad",   BIND(KPADInit),                               &KPADShutdown   },
            { "ac",     BIND(ACInitialize),                           &ACFinalize     },
            { "fs",     BIND(FSInit),                                 &FSShutdown     },
            // { "bsp",    BIND(bspInitializeShimInterface),             []() { }        }
        }};
        // clang-format on

        static constexpr const char* DEFAULT_PATH = "fs:/vol/external01/lovepotion.wuhb";

        std::string getApplicationPath(const std::string& argv0)
        {
            if (isDefaultEnvironment(argv0) || argv0 == "root.rpx")
                return DEFAULT_PATH;

            OSDynLoad_Module module;
            const auto type  = OS_DYNLOAD_EXPORT_FUNC;
            const char* name = "RL_GetPathOfRunningExecutable";

            if (OSDynLoad_Acquire("homebrew_rpx_loader", &module) != OS_DYNLOAD_OK)
                return DEFAULT_PATH;

            char path[256] = {};

            bool (*RL_GetPathOfRunningExecutable)(char*, uint32_t);
            auto** function = reinterpret_cast<void**>(&RL_GetPathOfRunningExecutable);

            if (OSDynLoad_FindExport(module, type, name, function) == OS_DYNLOAD_OK)
            {
                if (RL_GetPathOfRunningExecutable(path, sizeof(path)) == 0)
                {
                    OSDynLoad_Release(module);
                    return path;
                }
            }

            OSDynLoad_Release(module);
            return DEFAULT_PATH;
        }

        void errorHandler(const std::string& message)
        {
            WHBProcInit();
            VPADInit();

            WHBLogConsoleInit();
            WHBLogConsoleSetColor(0x00000000);
            WHBLogPrintf("Error:\n\n%s\n", message.c_str());
            WHBLogPrintf("Press + to quit.\n");

            VPADStatus status {};

            while (WHBProcIsRunning())
            {
                VPADRead(VPAD_CHAN_0, &status, 1, nullptr);
                if (status.trigger & VPAD_BUTTON_PLUS)
                    break;

                WHBLogConsoleDraw();

                OSSleepTicks(OSMillisecondsToTicks(100));
            }

            WHBLogConsoleFree();
        }

        int initialize()
        {
            const auto exceptionMode = OS_EXCEPTION_MODE_GLOBAL_ALL_CORES;
            OSSetExceptionCallbackEx(exceptionMode, OS_EXCEPTION_TYPE_DSI, handleDSIFatal);
            OSSetExceptionCallbackEx(exceptionMode, OS_EXCEPTION_TYPE_ISI, handleISIFatal);
            OSSetExceptionCallbackEx(exceptionMode, OS_EXCEPTION_TYPE_PROGRAM, handleProgramFatal);

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

            WPADEnableWiiRemote(true);
            WPADEnableURCC(true);

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

        bool run(lua_State* L, int argc, int* nres)
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

        void shutdown()
        {
            for (auto it = services.rbegin(); it != services.rend(); ++it)
                it->exit();
        }
    } // namespace platform
} // namespace love
