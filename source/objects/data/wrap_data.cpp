#include "objects/data/wrap_data.h"

using namespace love;

int Wrap_Data::GetPointer(lua_State* L)
{
    Data* self = Wrap_Data::CheckData(L, 1);

    lua_pushlightuserdata(L, self->GetData());

    return 1;
}

int Wrap_Data::GetSize(lua_State* L)
{
    Data* self = Wrap_Data::CheckData(L, 1);

    lua_pushnumber(L, self->GetSize());

    return 1;
}

int Wrap_Data::GetString(lua_State* L)
{
    Data* self = Wrap_Data::CheckData(L, 1);

    lua_pushlstring(L, (const char*)self->GetData(), self->GetSize());

    return 1;
}

Data* Wrap_Data::CheckData(lua_State* L, int index)
{
    return Luax::CheckType<Data>(L, index);
}

// clang-format off
const luaL_Reg Wrap_Data::functions[4] =
{
    { "getPointer", Wrap_Data::GetPointer },
    { "getSize",    Wrap_Data::GetSize    },
    { "getString",  Wrap_Data::GetString  },
    { 0,            0                     }
};
// clang-format on

int Wrap_Data::Register(lua_State* L)
{
    return Luax::RegisterType(L, &Data::type, functions, nullptr);
}
