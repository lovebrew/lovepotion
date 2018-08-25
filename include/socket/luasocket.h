#pragma once

namespace LuaSocket
{
    int Preload(lua_State * L);

    int InitSocket(lua_State * L);
    
    int InitHTTP(lua_State * L);

    void Close();
}