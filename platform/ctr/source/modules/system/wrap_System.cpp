#include "modules/system/wrap_System.hpp"

using namespace love;

#define instance() Module::getInstance<System>(Module::M_SYSTEM)

int Wrap_System::getPlayCoins(lua_State* L)
{
    int coins = 0;

    luax_catchexcept(L, [&]() { coins = instance()->getPlayCoins(); });
    lua_pushinteger(L, coins);

    return 1;
}

int Wrap_System::setPlayCoins(lua_State* L)
{
    int coins = luaL_checkinteger(L, 1);

    luax_catchexcept(L, [&]() { instance()->setPlayCoins(coins); });

    return 0;
}
