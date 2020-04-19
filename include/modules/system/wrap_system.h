#pragma once

#include "modules/system/system.h"

namespace Wrap_System
{
    int GetOS(lua_State * L);

    int GetProcessorCount(lua_State * L);

    int GetPowerInfo(lua_State * L);

    int GetNetworkInfo(lua_State * L);

    int GetLanguage(lua_State * L);

    int GetUsername(lua_State * L);

    int Register(lua_State * L);
}
