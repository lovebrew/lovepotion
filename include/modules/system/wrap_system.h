#pragma once

namespace Wrap_System
{
    int GetOS(lua_State * L);

    int GetProcessorCount(lua_State * L);

    int GetPowerInfo(lua_State * L);

    int GetNetworkInfo(lua_State * L);

    int GetLanguage(lua_State * L);

    int GetRegion(lua_State * L);

    int GetModel(lua_State * L);

    int GetUsername(lua_State * L);

    int GetVersion(lua_State * L);

    int GetFriendCode(lua_State * L);

    int Register(lua_State * L);
}
