#pragma once

namespace LuaSocket
{
    void Initialize();

    int InitSocket(lua_State * L);
    
    int InitHTTP(lua_State * L);

    void Exit();
}