#include "common/luax.hpp"

#include "modules/love/love.hpp"

#include <3ds.h>

using namespace love;

template<>
void love::PreInit<Console::CTR>()
{}

template<>
bool love::MainLoop<Console::CTR>(lua_State* L, int numArgs)
{
    return (luax::Resume(L, numArgs) == LUA_YIELD && aptMainLoop());
}

template<>
void love::OnExit<Console::CTR>()
{}
