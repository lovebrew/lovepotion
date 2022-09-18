#include <common/luax.hpp>

#include <modules/love/love.hpp>

#include <coreinit/debug.h>
#include <coreinit/title.h>
#include <whb/proc.h>

#include <sysapp/launch.h>
#include <utilities/log/logfile.h>

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
    int resume = luax::Resume(L, numArgs);
    OSReport("lua_resume returned: %d, WHBProcIsRunning: %d", resume, WHBProcIsRunning());
    return (resume == LUA_YIELD && WHBProcIsRunning());
}

template<>
void love::OnExit<Console::CAFE>()
{
    if (WHBProcIsRunning())
    {
        SYSLaunchMenu();
        while (WHBProcIsRunning())
        {}
    }

    WHBProcShutdown();
}
