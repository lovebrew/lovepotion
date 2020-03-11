#include "common/runtime.h"
#include "objects/source/wrap_source.h"

using namespace love;

Source * Wrap_Source::CheckSource(lua_State * L, int index)
{
    return Luax::CheckType<Source>(L, index);
}

int Wrap_Source::Register(lua_State * L)
{
    luaL_Reg reg[] =
    {
        { 0, 0 }
    };

    return Luax::RegisterType(L, &Source::type, reg, nullptr);
}
