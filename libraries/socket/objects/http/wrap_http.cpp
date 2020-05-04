#include "common/runtime.h"
#include "socket/objects/http/wrap_http.h"

int Wrap_HTTP::Request(lua_State * L)
{
    HTTP self;
    HTTP::Options options;

    if (lua_isstring(L, 1))
        options.url = luaL_checkstring(L, 1);
    else if (lua_istable(L, 1))
    {
        lua_getfield(L, 1, "url");
        lua_getfield(L, 1, "redirect");
        lua_getfield(L, 1, "method");
        lua_getfield(L, 1, "headers");

        options.url = luaL_checkstring(L, -4);
        options.redirect = lua_toboolean(L, -3);
        options.method = luaL_optstring(L, -2, "GET");

        // TODO: handle headers
        if (lua_istable(L, -1))
        {
            /* get table length */
            size_t length = lua_objlen(L, -1);

            for (size_t i = 0; i < length; i++)
            {}
        }
    }


    HTTP::Response chunk = self.Request(options);

    lua_pushlstring(L, chunk.buffer.data(), chunk.buffer.size());
    lua_pushnumber(L, chunk.code);

    lua_createtable(L, 0, chunk.headers.size());

    for (auto item : chunk.headers)
    {
        lua_pushlstring(L, item.value.data(), item.value.size());
        lua_setfield(L, -2, item.key.c_str());
    }

    chunk.buffer.clear();

    return 3;
}
