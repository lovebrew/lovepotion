#include "common/error.hpp"

#include "modules/data/wrap_ByteData.hpp"
#include "modules/data/wrap_Data.hpp"

using namespace love;

int Wrap_ByteData::clone(lua_State* L)
{
    auto* self      = luax_checkbytedata(L, 1);
    ByteData* clone = nullptr;

    luax_catchexcept(L, [&] { clone = new ByteData(*self); });

    luax_pushtype(L, clone);
    clone->release();

    return 1;
}

int Wrap_ByteData::setString(lua_State* L)
{
    auto* self = luax_checkbytedata(L, 1);

    size_t size        = 0;
    const char* string = luaL_checklstring(L, 2, &size);
    int64_t offset     = luaL_optnumber(L, 3, 0);

    size = std::min(size, self->getSize());

    if (size == 0)
        return 0;

    if (offset < 0 || offset + (int64_t)size > (int64_t)self->getSize())
        return luaL_error(L, E_INVALID_OFFSET_AND_SIZE);

    std::memcpy((char*)self->getData() + (size_t)offset, string, size);

    return 0;
}

template<typename T>
static int wrap_ByteData_setT(lua_State* L)
{
    auto* self     = luax_checkbytedata(L, 1);
    int64_t offset = luaL_checkinteger(L, 2);

    bool isTable = lua_type(L, 3) == LUA_TTABLE;
    int argc     = std::max(1, isTable ? (int)luax_objlen(L, 3) : lua_gettop(L) - 2);

    if (offset < 0 || offset + sizeof(T) * argc > (int64_t)self->getSize())
        return luaL_error(L, E_INVALID_OFFSET_AND_SIZE);

    auto data       = (uint8_t*)self->getData() + offset;
    const auto size = sizeof(T);

    if (isTable)
    {
        for (int index = 0; index < argc; index++)
        {
            lua_rawgeti(L, 3, index + 1);
            T value = (T)luaL_checknumber(L, -1);
            std::memcpy(data + (index * size), &value, size);
            lua_pop(L, 1);
        }
    }
    else
    {
        for (int index = 0; index < argc; index++)
        {
            T value = (T)luaL_checknumber(L, index + 3);
            std::memcpy(data + (index * size), &value, size);
        }
    }

    return 0;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",     Wrap_ByteData::clone         },
    { "setString", Wrap_ByteData::setString     },
    { "setFloat",  wrap_ByteData_setT<float>    },
    { "setDouble", wrap_ByteData_setT<double>   },
    { "setInt8",   wrap_ByteData_setT<int8_t>   },
    { "setUInt8",  wrap_ByteData_setT<uint8_t>  },
    { "setInt16",  wrap_ByteData_setT<int16_t>  },
    { "setUInt16", wrap_ByteData_setT<uint16_t> },
    { "setInt32",  wrap_ByteData_setT<int32_t>  },
    { "setUInt32", wrap_ByteData_setT<uint32_t> }
};
// clang-format on

namespace love
{
    ByteData* luax_checkbytedata(lua_State* L, int index)
    {
        return luax_checktype<ByteData>(L, index);
    }

    int open_bytedata(lua_State* L)
    {
        return luax_register_type(L, &ByteData::type, Wrap_Data::functions, functions);
    }
} // namespace love
