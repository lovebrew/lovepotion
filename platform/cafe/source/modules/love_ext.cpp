#include <common/luax.hpp>

#include <modules/love/love.hpp>

#include <coreinit/debug.h>
#include <coreinit/title.h>
#include <proc_ui/procui.h>

#include <whb/proc.h>

#include <sysapp/launch.h>

using namespace love;

template<>
void love::PreInit<Console::CAFE>()
{
    WHBProcInit();
}

template<>
bool love::IsRunningAppletMode<Console::CAFE>()
{
    return false;
}

template<>
bool love::MainLoop<Console::CAFE>(lua_State* L, int numArgs)
{
    return (luax::Resume(L, numArgs) == LUA_YIELD && WHBProcIsRunning());
}

template<>
void love::OnExit<Console::CAFE>()
{
    if (ProcUIIsRunning())
    {
        SYSLaunchMenu();
        while (WHBProcIsRunning())
        {}
    }

    if (!ProcUIInShutdown())
        WHBProcShutdown();
}
