#include "common/luax.hpp"

#include "modules/love/love.hpp"

#include <3ds.h>

template<>
bool love::MainLoop<love::Console::CTR>(lua_State* L, int numArgs)
{
    return (luax::Resume(L, numArgs) == LUA_YIELD && aptMainLoop());
}
