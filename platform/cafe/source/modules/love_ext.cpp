#include <common/luax.hpp>

#include <modules/love/love.hpp>

#include <coreinit/title.h>

#include <whb/gfx.h>
#include <whb/proc.h>

#include <sysapp/launch.h>

using namespace love;

template<>
void love::PreInit<Console::CAFE>()
{
    WHBProcInit();
    WHBGfxInit();
}

template<>
bool love::MainLoop<Console::CAFE>(lua_State* L, int numArgs)
{
    return (luax::Resume(L, numArgs) == LUA_YIELD && WHBProcIsRunning());
}

template<>
int love::Quit<Console::CAFE>(lua_State* L)
{
    SYSLaunchMenu();
    return 0;
}

template<>
int love::Restart<Console::CAFE>(lua_State* L)
{
    _SYSLaunchTitleWithStdArgsInNoSplash(OSGetTitleID(), nullptr);
    return 0;
}

template<>
void love::OnExit<Console::CAFE>()
{
    WHBGfxShutdown();
    WHBProcShutdown();
}
