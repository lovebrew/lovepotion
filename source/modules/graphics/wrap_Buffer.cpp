/**
 * Copyright (c) 2006-2025 LOVE Development Team
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 **/

#include "modules/graphics/wrap_Buffer.hpp"
#include "common/Data.hpp"
#include <limits>

using namespace love;

#define E_BUFFER_ARRAY_FLAT_ARRAY                                                   \
    "Array length in flat array variant of Buffer:setArrayData must be a multiple " \
    "of the total number of components (%d)"
#define E_BUFFER_TOO_MANY_ELEMENTS "Too many array elements (expected at most %d, got %d)"

int Wrap_Buffer::setArrayData(lua_State* L)
{
    auto* self           = luax_checkbuffer(L, 1);
    int sourceIndex      = (int)luaL_optnumber(L, 3, 1) - 1;
    int destinationIndex = (int)luaL_optnumber(L, 4, 1) - 1;

    if (sourceIndex < 0)
        return luaL_error(L, "Source start index must be at least 1.");

    int count = -1;
    if (!lua_isnoneornil(L, 5))
    {
        count = (int)luaL_checknumber(L, 5);
        if (count <= 0)
            return luaL_error(L, "Element count must be greater than 0.");
    }

    size_t stride       = self->getArrayStride();
    size_t bufferOffset = destinationIndex * stride;
    int arrayLength     = (int)self->getArrayLength();

    if (destinationIndex >= arrayLength || destinationIndex < 0)
        return luaL_error(L, "Invalid buffer start index (must be between 1 and %d)", arrayLength);

    if (luax_istype(L, 2, Data::type))
    {
        auto* data          = luax_checktype<Data>(L, 2);
        int dataArrayLength = data->getSize() / stride;

        if (sourceIndex >= dataArrayLength)
            return luaL_error(L, "Invalid data start index (must be between 1 and %d)", dataArrayLength);

        int maxCount = std::min(dataArrayLength - sourceIndex, arrayLength - destinationIndex);

        if (count < 0)
            count = maxCount;

        if (count > maxCount)
            return luaL_error(L, "Too many array elements (expected at most %d, got %d)", maxCount, count);

        size_t dataOffset      = sourceIndex * stride;
        size_t dataSize        = std::min(data->getSize() - dataOffset, count * stride);
        const void* sourceData = (const uint8_t*)data->getData() + dataOffset;
        self->fill(bufferOffset, dataSize, sourceData);
        return 0;
    }

    const auto& members = self->getDataMembers();
    int ncomponents     = 0;
    for (const auto& member : members)
        ncomponents += member.info.components;

    luaL_checktype(L, 2, LUA_TTABLE);
    int length = (int)luax_objlen(L, 2);

    lua_rawgeti(L, 2, 1);
    bool tableOfTables = lua_istable(L, -1);
    lua_pop(L, 1);

    if (!tableOfTables)
    {
        if (length % ncomponents != 0)
            return luaL_error(L, E_BUFFER_ARRAY_FLAT_ARRAY, ncomponents);
        length /= ncomponents;
    }

    if (sourceIndex >= length)
        return luaL_error(L, "Invalid data start index (must be between 1 and %d)", length);

    count = count >= 0 ? std::min(count, length - sourceIndex) : length - sourceIndex;

    if (destinationIndex + count > arrayLength)
        return luaL_error(L, E_BUFFER_TOO_MANY_ELEMENTS, arrayLength - destinationIndex, count);

    char* data = (char*)self->map(BufferBase::MAP_WRITE_INVALIDATE, bufferOffset, count * stride);

    if (tableOfTables)
    {
        for (int index = sourceIndex; index < count; index++)
        {
            lua_rawgeti(L, 2, index + 1);
            luaL_checktype(L, -1, LUA_TTABLE);

            for (int j = 1; j <= ncomponents; j++)
                lua_rawgeti(L, -j, j);

            int idx = -ncomponents;
            for (const auto& member : members)
            {
                luax_writebufferdata(L, idx, member.declaration.format, data + member.offset);
                idx += member.info.components;
            }

            lua_pop(L, ncomponents + 1);
            data += stride;
        }
    }
    else
    {
        for (int index = sourceIndex; index < count; index++)
        {
            for (int j = 1; j <= ncomponents; j++)
                lua_rawgeti(L, 2, index * ncomponents + j);

            int idx = -ncomponents;
            for (const auto& member : members)
            {
                luax_writebufferdata(L, idx, member.declaration.format, data + member.offset);
                idx += member.info.components;
            }

            lua_pop(L, ncomponents);
            data += stride;
        }
    }

    self->unmap(bufferOffset, count * stride);

    return 0;
}

int Wrap_Buffer::clear(lua_State* L)
{
    auto* self = luax_checkbuffer(L, 1);

    size_t offset = 0;
    size_t size   = self->getSize();

    if (!lua_isnoneornil(L, 2))
    {
        auto offsetp = luaL_checknumber(L, 2);
        auto sizep   = luaL_checknumber(L, 3);

        if (offsetp < 0 || sizep < 0)
            return luaL_error(L, "Offset and size parameters cannot be negative.");

        offset = (size_t)offsetp;
        size   = (size_t)sizep;
    }

    luax_catchexcept(L, [&]() { self->clear(offset, size); });

    return 0;
}

int Wrap_Buffer::getElementCount(lua_State* L)
{
    auto* self = luax_checkbuffer(L, 1);
    lua_pushinteger(L, self->getArrayLength());

    return 0;
}

int Wrap_Buffer::getElementStride(lua_State* L)
{
    auto* self = luax_checkbuffer(L, 1);
    lua_pushinteger(L, self->getArrayStride());

    return 0;
}

int Wrap_Buffer::getSize(lua_State* L)
{
    auto* self = luax_checkbuffer(L, 1);
    lua_pushinteger(L, self->getSize());

    return 0;
}

int Wrap_Buffer::getFormat(lua_State* L)
{
    auto* self          = luax_checkbuffer(L, 1);
    const auto& members = self->getDataMembers();

    lua_createtable(L, (int)members.size(), 0);

    for (size_t index = 0; index < members.size(); index++)
    {
        const auto& member = members[index];
        lua_createtable(L, 0, 4);

        lua_pushstring(L, member.declaration.name.c_str());
        lua_setfield(L, -2, "name");

        std::string_view format = "unknown";
        getConstant(member.declaration.format, format);
        luax_pushstring(L, format);
        lua_setfield(L, -2, "format");

        lua_pushinteger(L, member.declaration.arrayLength);
        lua_setfield(L, -2, "arraylength");

        lua_pushinteger(L, member.declaration.bindingLocation);
        lua_setfield(L, -2, "location");

        lua_pushinteger(L, member.offset);
        lua_setfield(L, -2, "offset");

        lua_pushinteger(L, member.size);
        lua_setfield(L, -2, "size");

        lua_rawseti(L, -2, index + 1);
    }

    return 1;
}

int Wrap_Buffer::isBufferType(lua_State* L)
{
    auto* self       = luax_checkbuffer(L, 1);
    const char* type = luaL_checkstring(L, 2);

    BufferUsage bufferUsage = BUFFERUSAGE_MAX_ENUM;
    if (!getConstant(type, bufferUsage))
        return luax_enumerror(L, "buffer type", BufferUsages, type);

    luax_pushboolean(L, (self->getUsageFlags() & (1 << bufferUsage)) != 0);
    return 1;
}

int Wrap_Buffer::getDebugName(lua_State* L)
{
    auto* self            = luax_checkbuffer(L, 1);
    const auto& debugName = self->getDebugName();

    if (debugName.empty())
        lua_pushnil(L);
    else
        luax_pushstring(L, debugName);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "setArrayData",     Wrap_Buffer::setArrayData     },
    { "clear",            Wrap_Buffer::clear            },
    { "getElementCount",  Wrap_Buffer::getElementCount  },
    { "getElementStride", Wrap_Buffer::getElementStride },
    { "getSize",          Wrap_Buffer::getSize          },
    { "getFormat",        Wrap_Buffer::getFormat        },
    { "isBufferType",     Wrap_Buffer::isBufferType     },
    { "getDebugName",     Wrap_Buffer::getDebugName     }
};
// clang-format on

namespace love
{
    static const double defaultComponents[] = { 0.0, 0.0, 0.0, 1.0 };

    template<typename T>
    static inline size_t writeData(lua_State* L, int startidx, int components, char* data)
    {
        auto componentdata = (T*)data;

        for (int i = 0; i < components; i++)
            componentdata[i] = (T)(luaL_optnumber(L, startidx + i, defaultComponents[i]));

        return sizeof(T) * components;
    }

    template<typename T>
    static inline size_t writeSNormData(lua_State* L, int startidx, int components, char* data)
    {
        auto componentdata    = (T*)data;
        constexpr auto maxval = std::numeric_limits<T>::max();

        for (int i = 0; i < components; i++)
            componentdata[i] =
                (T)(luax_optnumberclamped(L, startidx + i, -1.0, 1.0, defaultComponents[i]) * maxval);

        return sizeof(T) * components;
    }

    template<typename T>
    static inline size_t writeUNormData(lua_State* L, int startidx, int components, char* data)
    {
        auto componentdata    = (T*)data;
        constexpr auto maxval = std::numeric_limits<T>::max();

        for (int i = 0; i < components; i++)
            componentdata[i] = (T)(luax_optnumberclamped01(L, startidx + i, 1.0) * maxval);

        return sizeof(T) * components;
    }

    template<typename T>
    static inline size_t writeDataRequired(lua_State* L, int startidx, int components, char* data)
    {
        auto componentdata = (T*)data;

        for (int i = 0; i < components; i++)
            componentdata[i] = (T)(luaL_checknumber(L, startidx + i));

        return sizeof(T) * components;
    }

    void luax_writebufferdata(lua_State* L, int startidx, DataFormat format, char* data)
    {
        switch (format)
        {
            case DATAFORMAT_FLOAT:
                writeData<float>(L, startidx, 1, data);
                break;
            case DATAFORMAT_FLOAT_VEC2:
                writeData<float>(L, startidx, 2, data);
                break;
            case DATAFORMAT_FLOAT_VEC3:
                writeData<float>(L, startidx, 3, data);
                break;
            case DATAFORMAT_FLOAT_VEC4:
                writeData<float>(L, startidx, 4, data);
                break;

            case DATAFORMAT_FLOAT_MAT2X2:
                writeDataRequired<float>(L, startidx, 4, data);
                break;
            case DATAFORMAT_FLOAT_MAT2X3:
                writeDataRequired<float>(L, startidx, 6, data);
                break;
            case DATAFORMAT_FLOAT_MAT2X4:
                writeDataRequired<float>(L, startidx, 8, data);
                break;

            case DATAFORMAT_FLOAT_MAT3X2:
                writeDataRequired<float>(L, startidx, 6, data);
                break;
            case DATAFORMAT_FLOAT_MAT3X3:
                writeDataRequired<float>(L, startidx, 9, data);
                break;
            case DATAFORMAT_FLOAT_MAT3X4:
                writeDataRequired<float>(L, startidx, 12, data);
                break;

            case DATAFORMAT_FLOAT_MAT4X2:
                writeDataRequired<float>(L, startidx, 8, data);
                break;
            case DATAFORMAT_FLOAT_MAT4X3:
                writeDataRequired<float>(L, startidx, 12, data);
                break;
            case DATAFORMAT_FLOAT_MAT4X4:
                writeDataRequired<float>(L, startidx, 16, data);
                break;

            case DATAFORMAT_INT32:
                writeData<int32_t>(L, startidx, 1, data);
                break;
            case DATAFORMAT_INT32_VEC2:
                writeData<int32_t>(L, startidx, 2, data);
                break;
            case DATAFORMAT_INT32_VEC3:
                writeData<int32_t>(L, startidx, 3, data);
                break;
            case DATAFORMAT_INT32_VEC4:
                writeData<int32_t>(L, startidx, 4, data);
                break;

            case DATAFORMAT_UINT32:
                writeData<uint32_t>(L, startidx, 1, data);
                break;
            case DATAFORMAT_UINT32_VEC2:
                writeData<uint32_t>(L, startidx, 2, data);
                break;
            case DATAFORMAT_UINT32_VEC3:
                writeData<uint32_t>(L, startidx, 3, data);
                break;
            case DATAFORMAT_UINT32_VEC4:
                writeData<uint32_t>(L, startidx, 4, data);
                break;

            case DATAFORMAT_SNORM8_VEC4:
                writeSNormData<int8_t>(L, startidx, 4, data);
                break;
            case DATAFORMAT_UNORM8_VEC4:
                writeUNormData<uint8_t>(L, startidx, 4, data);
                break;
            case DATAFORMAT_INT8_VEC4:
                writeData<int8_t>(L, startidx, 4, data);
                break;
            case DATAFORMAT_UINT8_VEC4:
                writeData<uint8_t>(L, startidx, 4, data);
                break;

            case DATAFORMAT_SNORM16_VEC2:
                writeSNormData<int16_t>(L, startidx, 2, data);
                break;
            case DATAFORMAT_SNORM16_VEC4:
                writeSNormData<int16_t>(L, startidx, 4, data);
                break;

            case DATAFORMAT_UNORM16_VEC2:
                writeUNormData<uint16_t>(L, startidx, 2, data);
                break;
            case DATAFORMAT_UNORM16_VEC4:
                writeUNormData<uint16_t>(L, startidx, 4, data);
                break;

            case DATAFORMAT_INT16_VEC2:
                writeData<int16_t>(L, startidx, 2, data);
                break;
            case DATAFORMAT_INT16_VEC4:
                writeData<int16_t>(L, startidx, 4, data);
                break;

            case DATAFORMAT_UINT16:
                writeData<uint16_t>(L, startidx, 1, data);
                break;
            case DATAFORMAT_UINT16_VEC2:
                writeData<uint16_t>(L, startidx, 2, data);
                break;
            case DATAFORMAT_UINT16_VEC4:
                writeData<uint16_t>(L, startidx, 4, data);
                break;

            default:
                break;
        }
    }

    template<typename T>
    static inline size_t readData(lua_State* L, int components, const char* data)
    {
        const auto componentdata = (const T*)data;

        for (int i = 0; i < components; i++)
            lua_pushnumber(L, (lua_Number)componentdata[i]);

        return sizeof(T) * components;
    }

    template<typename T>
    static inline size_t readSNormData(lua_State* L, int components, const char* data)
    {
        const auto componentdata = (const T*)data;
        constexpr auto maxval    = std::numeric_limits<T>::max();

        for (int i = 0; i < components; i++)
            lua_pushnumber(L, std::max(-1.0, (lua_Number)componentdata[i] / (lua_Number)maxval));

        return sizeof(T) * components;
    }

    template<typename T>
    static inline size_t readUNormData(lua_State* L, int components, const char* data)
    {
        const auto componentdata = (const T*)data;
        constexpr auto maxval    = std::numeric_limits<T>::max();

        for (int i = 0; i < components; i++)
            lua_pushnumber(L, (lua_Number)componentdata[i] / (lua_Number)maxval);

        return sizeof(T) * components;
    }

    void luax_readbufferdata(lua_State* L, DataFormat format, const char* data)
    {
        switch (format)
        {
            case DATAFORMAT_FLOAT:
                readData<float>(L, 1, data);
                break;
            case DATAFORMAT_FLOAT_VEC2:
                readData<float>(L, 2, data);
                break;
            case DATAFORMAT_FLOAT_VEC3:
                readData<float>(L, 3, data);
                break;
            case DATAFORMAT_FLOAT_VEC4:
                readData<float>(L, 4, data);
                break;

            case DATAFORMAT_INT32:
                readData<int32_t>(L, 1, data);
                break;
            case DATAFORMAT_INT32_VEC2:
                readData<int32_t>(L, 2, data);
                break;
            case DATAFORMAT_INT32_VEC3:
                readData<int32_t>(L, 3, data);
                break;
            case DATAFORMAT_INT32_VEC4:
                readData<int32_t>(L, 4, data);
                break;

            case DATAFORMAT_UINT32:
                readData<uint32_t>(L, 1, data);
                break;
            case DATAFORMAT_UINT32_VEC2:
                readData<uint32_t>(L, 2, data);
                break;
            case DATAFORMAT_UINT32_VEC3:
                readData<uint32_t>(L, 3, data);
                break;
            case DATAFORMAT_UINT32_VEC4:
                readData<uint32_t>(L, 4, data);
                break;

            case DATAFORMAT_SNORM8_VEC4:
                readSNormData<int8_t>(L, 4, data);
                break;
            case DATAFORMAT_UNORM8_VEC4:
                readUNormData<uint8_t>(L, 4, data);
                break;
            case DATAFORMAT_INT8_VEC4:
                readData<int8_t>(L, 4, data);
                break;
            case DATAFORMAT_UINT8_VEC4:
                readData<uint8_t>(L, 4, data);
                break;

            case DATAFORMAT_SNORM16_VEC2:
                readSNormData<int16_t>(L, 2, data);
                break;
            case DATAFORMAT_SNORM16_VEC4:
                readSNormData<int16_t>(L, 4, data);
                break;

            case DATAFORMAT_UNORM16_VEC2:
                readUNormData<uint16_t>(L, 2, data);
                break;
            case DATAFORMAT_UNORM16_VEC4:
                readUNormData<uint16_t>(L, 4, data);
                break;

            case DATAFORMAT_INT16_VEC2:
                readData<int16_t>(L, 2, data);
                break;
            case DATAFORMAT_INT16_VEC4:
                readData<int16_t>(L, 4, data);
                break;

            case DATAFORMAT_UINT16:
                readData<uint16_t>(L, 1, data);
                break;
            case DATAFORMAT_UINT16_VEC2:
                readData<uint16_t>(L, 2, data);
                break;
            case DATAFORMAT_UINT16_VEC4:
                readData<uint16_t>(L, 4, data);
                break;

            default:
                break;
        }
    }

    BufferBase* luax_checkbuffer(lua_State* L, int index)
    {
        return luax_checktype<BufferBase>(L, index);
    }

    int open_buffer(lua_State* L)
    {
        return luax_register_type(L, &BufferBase::type, functions);
    }
} // namespace love
