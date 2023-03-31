#include "objects/data/wrap_data.hpp"

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
    auto* self     = Wrap_Data::CheckData(L, 1);
    int64_t offset = (int64_t)luaL_optnumber(L, 2, 0);

    int64_t size = lua_isnoneornil(L, 3) ? ((int64_t)self->GetSize() - offset)
                                         : (int64_t)luaL_checknumber(L, 3);

    if (size <= 0)
        return luaL_error(L, "Invalid size parameter (must be greater than 0)");

    if (offset < 0 || offset + size > (int64_t)self->GetSize())
        return luaL_error(L,
                          "The given offset and size parameters don't fit within the Data's size.");

    auto data = (const char*)self->GetData() + offset;

    lua_pushlstring(L, data, size);

    return 1;
}

template<typename T>
static int Wrap_Data_GetT(lua_State* L)
{
    auto* self     = Wrap_Data::CheckData(L, 1);
    int64_t offset = (int64_t)luaL_checknumber(L, 2);
    int count      = (int)luaL_optinteger(L, 3, 1);

    if (count <= 0)
        return luaL_error(L, "Invalid count parameter (must be greater than 0)");

    if (offset < 0 || offset + sizeof(T) * count > self->GetSize())
        return luaL_error(
            L, "The given offset and count parameters don't fit within the Data's size.");

    auto data = (const T*)((uint8_t*)self->GetData() + offset);

    for (int i = 0; i < count; i++)
        lua_pushnumber(L, (lua_Number)data[i]);

    return count;
}

Data* Wrap_Data::CheckData(lua_State* L, int index)
{
    return luax::CheckType<Data>(L, index);
}

// clang-format off
const luaL_Reg Wrap_Data::functions[] =
{
    { "getPointer", Wrap_Data::GetPointer    },
    { "getSize",    Wrap_Data::GetSize       },
    { "getString",  Wrap_Data::GetString     },
    { "getFloat",   Wrap_Data_GetT<float>    },
	{ "getDouble",  Wrap_Data_GetT<double>   },
	{ "getInt8",    Wrap_Data_GetT<int8_t>   },
	{ "getUInt8",   Wrap_Data_GetT<uint8_t>  },
	{ "getInt16",   Wrap_Data_GetT<int16_t>  },
	{ "getUInt16",  Wrap_Data_GetT<uint16_t> },
	{ "getInt32",   Wrap_Data_GetT<int32_t>  },
	{ "getUInt32",  Wrap_Data_GetT<uint32_t> },
	{ "getInt64",   Wrap_Data_GetT<int64_t>  },
	{ "getUInt64",  Wrap_Data_GetT<uint64_t> }
};
// clang-format on

int Wrap_Data::Register(lua_State* L)
{
    return luax::RegisterType(L, &Data::type, functions);
}
