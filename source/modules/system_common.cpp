#include "common/runtime.h"
#include "modules/system.h"

int System::Register(lua_State * L)
{
    System::Initialize();

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