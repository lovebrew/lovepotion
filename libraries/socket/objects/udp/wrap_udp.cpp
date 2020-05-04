#include "common/runtime.h"
#include "socket/objects/udp/wrap_udp.h"

using namespace love;

int Wrap_UDP::Create(lua_State * L)
{

}

int Register(lua_State * L)
{
    luaL_Reg funcs[] =
    {
        { 0, 0 }
    };

    LuaSocket::NewClass(L, "udp{connected}", funcs);
    LuaSocket::NewClass(L, "udp{unconnected}", funcs);

    return 0;
}
