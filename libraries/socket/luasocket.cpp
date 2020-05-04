#include "socket/luasocket.h"

int LuaSocket::Open(lua_State * L)
{
    lua_CFunction types[] =
    {
        Wrap_Socket::Register,
        //Wrap_UDP::Register,
        0
    };

    luaL_Reg reg[] =
    {
        { "udp", Wrap_UDP::Create },
        { 0, 0 },
    };

    return 0;
}

void LuaSocket::NewClass(lua_State * L, const char * classname, ...)
{
    luaL_newmetatable(L, classname);

    /* create __index table to place methods */
    lua_pushstring(L, "__index");    /* mt,"__index" */
    lua_newtable(L);                 /* mt,"__index",it */

    /* put class name into class metatable */
    lua_pushstring(L, "class");      /* mt,"__index",it,"class" */
    lua_pushstring(L, classname);    /* mt,"__index",it,"class",classname */
    lua_rawset(L, -3);               /* mt,"__index",it */

    /* pass all methods that start with _ to the metatable, and all others
    * to the index table
    */

    va_list funcs;
    va_start(funcs, classname);

    for (const luaL_Reg * func = va_arg(funcs, const luaL_Reg *); func; func = va_arg(funcs, const luaL_Reg *))
    {
        for (; func->name; func++)
        {
            lua_pushstring(L, func->name);
            lua_pushcfunction(L, func->func);
            lua_rawset(L, func->name[0] == '_' ? -5: -3);
        }
    }

    va_end(funcs);


    lua_rawset(L, -3);               /* mt */
    lua_pop(L, 1);
}

int LuaSocket::ToString(lua_State * L)
{
    char buffer[32];

    if (!lua_getmetatable(L, 1))
        return lua_error(L);

    lua_pushstring(L, "__index");

    lua_gettable(L, -2);

    if (!lua_istable(L, -1))
        return lua_error(L);

    lua_pushstring(L, "class");
    lua_gettable(L, -2);

    if (!lua_isstring(L, -1))
        return lua_error(L);

    sprintf(buffer, "%p", lua_touserdata(L, 1));
    lua_pushfstring(L, "%s: %s", lua_tostring(L, -1), buffer);

    return 1;
}

int LuaSocket::OpenHTTP(lua_State * L)
{
    luaL_Reg reg[] =
    {
        { "request", Wrap_HTTP::Request },
        { 0, 0 },
    };

    luaL_newlib(L, reg);

    return 1;
}
