#include "modules/data/wrap_Data.hpp"
#include "common/error.hpp"

using namespace love;

int Wrap_Data::getString(lua_State* L)
{
    auto* self     = luax_checkdata(L, 1);
    int64_t offset = (int64_t)luaL_optnumber(L, 2, 0);

    int64_t size =
        lua_isnoneornil(L, 3) ? ((int64_t)self->getSize() - offset) : (int64_t)luaL_checknumber(L, 3);

    if (size <= 0)
        return luaL_error(L, E_INVALID_SIZE_PARAMETER);

    if (offset < 0 || offset + size > (int64_t)self->getSize())
        return luaL_error(L, E_INVALID_OFFSET_AND_SIZE);

    auto* data = (const char*)self->getData() + offset;
    lua_pushlstring(L, data, size);

    return 1;
}

int Wrap_Data::getSize(lua_State* L)
{
    auto* self = luax_checkdata(L, 1);

    lua_pushnumber(L, (lua_Number)self->getSize());

    return 1;
}

int Wrap_Data::getPointer(lua_State* L)
{
    auto* self = luax_checkdata(L, 1);

    lua_pushlightuserdata(L, self->getData());

    return 1;
}

int Wrap_Data::getFFIPointer(lua_State* L)
{
    lua_pushnil(L);

    return 1;
}

int Wrap_Data::performAtomic(lua_State* L)
{
    auto* self = luax_checkdata(L, 1);
    int error  = 0;

    {
        std::unique_lock lock(*self->getMutex());
        error = lua_pcall(L, lua_gettop(L) - 2, LUA_MULTRET, 0);
    }

    if (error != 0)
        return lua_error(L);

    return lua_gettop(L) - 1;
}

template<typename T>
static int wrap_Data_getT(lua_State* L)
{
    auto* self     = luax_checkdata(L, 1);
    int64_t offset = (int64_t)luaL_checknumber(L, 2);
    int count      = (int)luaL_optinteger(L, 3, 1);

    const auto size = sizeof(T);

    if (count <= 0)
        return luaL_error(L, E_INVALID_COUNT_PARAMETER);

    if (offset < 0 || offset + size * count > (int64_t)self->getSize())
        return luaL_error(L, E_INVALID_OFFSET_AND_SIZE);

    auto* data = (uint8_t*)self->getData() + offset;

    for (int i = 0; i < count; i++)
    {
        T value {};
        std::memcpy(&value, data + (i * size), size);
        lua_pushnumber(L, (lua_Number)value);
    }

    return count;
}

// clang-format off
luaL_Reg Wrap_Data::functions[] =
{
    { "getPointer",    Wrap_Data::getPointer     },
    { "getSize",       Wrap_Data::getSize        },
    { "getString",     Wrap_Data::getString      },
    { "getFloat",      wrap_Data_getT<float>     },
	{ "getDouble",     wrap_Data_getT<double>    },
	{ "getInt8",       wrap_Data_getT<int8_t>    },
	{ "getUInt8",      wrap_Data_getT<uint8_t>   },
	{ "getInt16",      wrap_Data_getT<int16_t>   },
	{ "getUInt16",     wrap_Data_getT<uint16_t>  },
	{ "getInt32",      wrap_Data_getT<int32_t>   },
	{ "getUInt32",     wrap_Data_getT<uint32_t>  },
    { "performAtomic", Wrap_Data::performAtomic  },
    { "getFFIPointer", Wrap_Data::getFFIPointer  }
};
// clang-format on

namespace love
{
    Data* luax_checkdata(lua_State* L, int idx)
    {
        return luax_checktype<Data>(L, idx);
    }

    int open_data(lua_State* L)
    {
        return luax_register_type(L, &Data::type, Wrap_Data::functions);
    }
} // namespace love
