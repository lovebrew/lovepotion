#include "objects/data/bytedata/wrap_bytedata.hpp"

#include <cstring>

using namespace love;

ByteData* Wrap_ByteData::CheckByteData(lua_State* L, int index)
{
    return luax::CheckType<ByteData>(L, index);
}

int Wrap_ByteData::Clone(lua_State* L)
{
    auto* self      = Wrap_ByteData::CheckByteData(L, 1);
    ByteData* clone = nullptr;

    luax::CatchException(L, [&]() { clone = self->Clone(); });

    luax::PushType(L, clone);
    clone->Release();

    return 1;
}

int Wrap_ByteData::SetString(lua_State* L)
{
    auto* self  = Wrap_ByteData::CheckByteData(L, 1);
    size_t size = 0;

    const char* string = luaL_checklstring(L, 2, &size);
    int64_t offset     = luaL_optnumber(L, 3, 0);

    size = std::min(size, self->GetSize());

    if (size == 0)
        return 0;

    if (offset < 0 || offset + size > (int64_t)self->GetSize())
        return luaL_error(L, "The given string offset and size don't fit within the Data's size.");

    std::memcpy((char*)self->GetData() + (size_t)offset, string, size);

    return 0;
}

template<typename T>
static int Wrap_ByteData_SetT(lua_State* L)
{
    auto* self     = Wrap_ByteData::CheckByteData(L, 1);
    int64_t offset = luaL_checknumber(L, 2);

    bool isTable = lua_type(L, 3) == LUA_TTABLE;
    int nargs    = std::max(1, isTable ? (int)luax::ObjectLength(L, 3) : lua_gettop(L) - 2);

    if (offset < 0 || offset + sizeof(T) * nargs > self->GetSize())
        return luaL_error(
            L, "The given offset and value parameters don't fit within the Data's size.");

    auto data = (T*)((uint8_t*)self->GetData() + offset);

    if (isTable)
    {
        for (int index = 0; index < nargs; index++)
        {
            lua_rawgeti(L, 3, index + 1);
            data[index] = (T)luaL_checknumber(L, -1);
            lua_pop(L, 1);
        }
    }
    else
    {
        for (int index = 0; index < nargs; index++)
            data[index] = (T)luaL_checknumber(L, 3 + index);
    }

    return 0;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",     Wrap_ByteData::Clone         },
    { "setString", Wrap_ByteData::SetString     },
    { "setFloat",  Wrap_ByteData_SetT<float>    },
	{ "setDouble", Wrap_ByteData_SetT<double>   },
	{ "setInt8",   Wrap_ByteData_SetT<int8_t>   },
	{ "setUInt8",  Wrap_ByteData_SetT<uint8_t>  },
	{ "setInt16",  Wrap_ByteData_SetT<int16_t>  },
	{ "setUInt16", Wrap_ByteData_SetT<uint16_t> },
	{ "setInt32",  Wrap_ByteData_SetT<int32_t>  },
	{ "setUInt32", Wrap_ByteData_SetT<uint32_t> },
	{ "setInt64",  Wrap_ByteData_SetT<int64_t>  },
	{ "setUInt64", Wrap_ByteData_SetT<uint64_t> }
};
// clang-format on

int Wrap_ByteData::Register(lua_State* L)
{
    return luax::RegisterType(L, &ByteData::type, Wrap_Data::functions, functions);
}
