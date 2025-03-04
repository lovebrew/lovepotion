#include "modules/graphics/wrap_Mesh.hpp"

#include "modules/graphics/Texture.tcc"
#include "modules/graphics/wrap_Texture.hpp"

#include <algorithm>

using namespace love;

int Wrap_Mesh::setVertices(lua_State* L)
{
    auto* self = luax_checkmesh(L, 1);
    int start  = luaL_optnumber(L, 3, 1) - 1;

    int count = -1;
    if (lua_isnoneornil(L, 4))
    {
        count = luaL_checknumber(L, 4);
        if (count <= 0)
            return luaL_error(L, "Vertex count must be greater than 0.");
    }

    size_t stride = self->getVertexStride();
    size_t offset = start * stride;
    int total     = self->getVertexCount();

    if (start >= total || start < 0)
        return luaL_error(L, "Invalid vertex start index (must be between 1 and %d).", total);

    if (luax_istype(L, 2, Data::type))
    {
        auto* data = luax_checktype<Data>(L, 2);
        count      = count >= 0 ? count : (total - start);

        if (start + count > total)
            return luaL_error(L, "Too many vertices (expected at most %d, got %d).", total - start, count);

        size_t dataSize = std::min(data->getSize(), count * stride);
        char* byteData  = (char*)self->getVertexData() + offset;

        std::memcpy(byteData, data->getData(), dataSize);

        self->setVertexDataModified(offset, dataSize);
        self->flush();

        return 0;
    }

    luaL_checktype(L, 2, LUA_TTABLE);
    int length = (int)luax_objlen(L, 2);

    count = count >= 0 ? std::min(count, length) : length;
    if (start + count > total)
        return luaL_error(L, "Too many vertices (expected at most %d, got %d).", total - start, count);

    return 0;
}

int Wrap_Mesh::getVertexCount(lua_State* L)
{
    auto* self = luax_checkmesh(L, 1);
    lua_pushinteger(L, self->getVertexCount());

    return 1;
}

int Wrap_Mesh::flush(lua_State* L)
{
    auto* self = luax_checkmesh(L, 1);

    self->flush();

    return 0;
}

int Wrap_Mesh::setVertexMap(lua_State* L)
{
    auto* self = luax_checkmesh(L, 1);

    if (lua_isnoneornil(L, 2))
    {
        luax_catchexcept(L, [&] { self->setVertexMap(); });
        return 0;
    }

    if (luax_istype(L, 2, Data::type))
    {
        auto* data = luax_totype<Data>(L, 2, Data::type);

        const char* indexTypeString = luaL_checkstring(L, 3);
        IndexDataType indexType;

        if (!getConstant(indexTypeString, indexType))
            return luax_enumerror(L, "index data type", IndexDataTypes, indexTypeString);

        size_t dataTypeSize = getIndexDataSize(indexType);
        int indexCount      = luaL_optinteger(L, 4, data->getSize() / dataTypeSize);

        if (indexCount < 1 || indexCount * dataTypeSize > data->getSize())
            return luaL_error(L, "Invalid index count: %d.", indexCount);

        luax_catchexcept(L, [&] { self->setVertexMap(indexType, data->getData(), indexCount); });

        return 0;
    }

    bool isTable = lua_istable(L, 2);
    int argc     = isTable ? luax_objlen(L, 2) : lua_gettop(L) - 1;

    std::vector<uint32_t> vertexMap {};
    vertexMap.reserve(argc);

    if (isTable)
    {
        for (int index = 0; index < argc; index++)
        {
            lua_rawgeti(L, 2, index + 1);
            vertexMap.push_back(uint32_t(luaL_checkinteger(L, -1) - 1));
            lua_pop(L, 1);
        }
    }
    else
    {
        for (int index = 0; index < argc; index++)
            vertexMap.push_back(uint32_t(luaL_checkinteger(L, index + 2) - 1));
    }

    luax_catchexcept(L, [&] { self->setVertexMap(vertexMap); });

    return 0;
}

int Wrap_Mesh::getVertexMap(lua_State* L)
{
    auto* self = luax_checkmesh(L, 1);

    std::vector<uint32_t> vertexMap {};
    bool hasVertexMap = false;

    luax_catchexcept(L, [&] { hasVertexMap = self->getVertexMap(vertexMap); });

    if (!hasVertexMap)
    {
        lua_pushnil(L);
        return 1;
    }

    int elementCount = (int)vertexMap.size();
    lua_createtable(L, elementCount, 0);

    for (int index = 0; index < elementCount; index++)
    {
        lua_pushinteger(L, lua_Integer(vertexMap[index]) + 1);
        lua_rawseti(L, -2, index + 1);
    }

    return 1;
}

int Wrap_Mesh::setTexture(lua_State* L)
{
    auto* self = luax_checkmesh(L, 1);

    if (lua_isnoneornil(L, 2))
        self->setTexture();
    else
    {
        auto* texture = luax_checktexture(L, 2);
        self->setTexture(texture);
    }

    return 0;
}

int Wrap_Mesh::getTexture(lua_State* L)
{
    auto* self    = luax_checkmesh(L, 1);
    auto* texture = self->getTexture();

    if (texture == nullptr)
        return 0;

    luax_pushtype(L, texture);

    return 1;
}

int Wrap_Mesh::setDrawMode(lua_State* L)
{
    auto* self = luax_checkmesh(L, 1);

    const char* modeString = luaL_checkstring(L, 2);
    PrimitiveType mode;

    if (!getConstant(modeString, mode))
        return luax_enumerror(L, "mesh draw mode", PrimitiveTypes, modeString);

    self->setDrawMode(mode);

    return 0;
}

int Wrap_Mesh::getDrawMode(lua_State* L)
{
    auto* self = luax_checkmesh(L, 1);
    auto mode  = self->getDrawMode();

    std::string_view name {};
    if (!getConstant(mode, name))
        return luaL_error(L, "Unknown mesh draw mode.");

    luax_pushstring(L, name);

    return 1;
}

int Wrap_Mesh::setDrawRange(lua_State* L)
{
    auto* self = luax_checkmesh(L, 1);

    if (lua_isnoneornil(L, 2))
        self->setDrawRange();
    else
    {
        int start = luaL_checkinteger(L, 2) - 1;
        int count = luaL_checkinteger(L, 3);

        luax_catchexcept(L, [&] { self->setDrawRange(start, count); });
    }

    return 0;
}

int Wrap_Mesh::getDrawRange(lua_State* L)
{
    auto* self = luax_checkmesh(L, 1);

    int start = 0;
    int count = 1;

    if (!self->getDrawRange(start, count))
        return 0;

    lua_pushinteger(L, start + 1);
    lua_pushinteger(L, count);

    return 2;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getVertexCount", Wrap_Mesh::getVertexCount },
    { "flush",          Wrap_Mesh::flush          },
    { "setVertexMap",   Wrap_Mesh::setVertexMap   },
    { "getVertexMap",   Wrap_Mesh::getVertexMap   },
    { "setTexture",     Wrap_Mesh::setTexture     },
    { "getTexture",     Wrap_Mesh::getTexture     },
    { "setDrawMode",    Wrap_Mesh::setDrawMode    },
    { "getDrawMode",    Wrap_Mesh::getDrawMode    },
    { "setDrawRange",   Wrap_Mesh::setDrawRange   },
    { "getDrawRange",   Wrap_Mesh::getDrawRange   }
};
// clang-format on

namespace love
{
    Mesh* luax_checkmesh(lua_State* L, int index)
    {
        return luax_checktype<Mesh>(L, index);
    }

    int open_mesh(lua_State* L)
    {
        return luax_register_type(L, &Mesh::type, functions);
    }
} // namespace love
