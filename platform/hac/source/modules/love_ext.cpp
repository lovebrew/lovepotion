#include "common/luax.hpp"
#include "modules/love/love.hpp"

#include <switch.h>

using namespace love;

template<>
void love::PreInit<Console::HAC>()
{}

template<>
void love::OnExit<Console::HAC>()
{}

template<>
bool love::IsRunningAppletMode<love::Console::HAC>()
{
    AppletType type = appletGetAppletType();

    bool isApplication = (type == AppletType_Application || type == AppletType_SystemApplication);

    if (isApplication)
        return false;

    const char* TITLE_TAKEOVER_ERROR = "Please run LÖVE Potion under "
                                       "Atmosphère title takeover.";

    ErrorApplicationConfig config;

    errorApplicationCreate(&config, TITLE_TAKEOVER_ERROR, NULL);
    errorApplicationShow(&config);

    return true;
}

template<>
bool love::MainLoop<love::Console::HAC>(lua_State* L, int numArgs)
{
    return luax::Resume(L, numArgs) == LUA_YIELD;
}
