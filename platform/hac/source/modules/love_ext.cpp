#include "common/luax.hpp"
#include "modules/love/love.hpp"

#include <switch.h>

using namespace love;

template<>
void love::PreInit<Console::HAC>()
{}

template<>
void love::OnExit<Console::HAC>()
{}

template<>
bool love::MainLoop<love::Console::HAC>(lua_State* L, int numArgs)
{
    return luax::Resume(L, numArgs) == LUA_YIELD;
}
