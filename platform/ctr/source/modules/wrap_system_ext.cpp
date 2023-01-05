#include <modules/system/wrap_system.hpp>
#include <modules/system_ext.hpp>

using namespace love;

#define instance() (Module::GetInstance<System<Console::Which>>(Module::M_SYSTEM))

int Wrap_System::SetPlayCoins(lua_State* L)
{
    int amount = luaL_checknumber(L, 1);

    luax::CatchException(L, [&]() { instance()->SetPlayCoins(amount); });

    return 0;
}

int Wrap_System::GetPlayCoins(lua_State* L)
{
    int amount = 0;

    luax::CatchException(L, [&]() { amount = instance()->GetPlayCoins(); });

    lua_pushnumber(L, amount);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getPlayCoins", Wrap_System::GetPlayCoins },
    { "setPlayCoins", Wrap_System::SetPlayCoins }
};
// clang-format on

std::span<const luaL_Reg> Wrap_System::extensions = functions;
