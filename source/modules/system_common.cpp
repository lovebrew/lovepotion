#include "common/runtime.h"
#include "modules/system.h"

//love.system.getOS
int System::GetOS(lua_State * L)
{
    lua_pushstring(L, "Horizon");

    return 1;
}

int System::Register(lua_State * L)
{
    luaL_Reg reg[] = 
    {
        { "getPowerInfo",      GetPowerInfo      },
        { "getProcessorCount", GetProcessorCount },
        { "getOS",             GetOS             },
        { "getLanguage",       GetLanguage       },
        { "getInternetStatus", GetInternetStatus },
        { "getRegion",         GetRegion         },
        { "getUsername",       GetUsername       },
        { 0, 0 },
    };

    luaL_newlib(L, reg);

    return 1;
}
