#include "modules/graphics/wrap_Mesh.hpp"
#include "modules/graphics/Texture.hpp"
#include "modules/graphics/wrap_Buffer.hpp"
#include "modules/graphics/wrap_Texture.hpp"

#include <algorithm>

using namespace love;

int Wrap_Mesh::setVertices(lua_State* L)
{
    auto* self = luax_checkmesh(L, 1);

    int start = luaL_optnumber(L, 3, 1) - 1;
    int count = -1;

    if (!lua_isnoneornil(L, 4))
    {
        count = (int)luaL_checknumber(L, 4);
        if (count <= 0)
            return luaL_error(L, "Vertex count must be greater than 0.");
    }

    size_t stride     = self->getVertexStride();
    size_t byteoffset = start * stride;
    int total         = (int)self->getVertexCount();

    if (start >= total || start < 0)
        return luaL_error(L, "Invalid vertex start index (must be between 1 and %d).", total);

    if (luax_istype(L, 2, Data::type))
    {
        auto* data = luax_checktype<Data>(L, 2);

        count = count >= 0 ? count : (total - start);
        if (start + count > total)
            return luaL_error(L, "Too many vertices (expected at most %d, got %d).", total - start, count);

        size_t size = std::min(data->getSize(), count * stride);
        char* bytes = (char*)self->getVertexData() + byteoffset;
        std::memcpy(bytes, data->getData(), size);

        self->setVertexDataModified(byteoffset, size);
        self->flush();
        return 0;
    }

    luaL_checktype(L, 2, LUA_TTABLE);
    int length = luax_objlen(L, 2);

    count = count >= 0 ? std::min(count, length) : length;
    if (start + count > total)
        return luaL_error(L, "Too many vertices (expected at most %d, got %d).", total - start, count);

    const auto& format = self->getVertexFormat();

    int numComponents = 0;
    for (const auto& member : format)
        numComponents += member.info.components;

    auto* data = (char*)self->getVertexData() + byteoffset;
    for (int i = 0; i < count; i++)
    {
        lua_rawgeti(L, 2, i + 1);
        luaL_checktype(L, -1, LUA_TTABLE);

        for (int j = 1; j <= numComponents; j++)
            lua_rawgeti(L, -j, j);

        int index = -numComponents;

        for (const auto& member : format)
        {
            luax_writebufferdata(L, index, member.declaration.format, data + member.offset);
            index += member.info.components;
        }

        lua_pop(L, numComponents + 1);
        data += stride;
    }

    self->setVertexDataModified(byteoffset, count * stride);
    self->flush();

    return 0;
}

int Wrap_Mesh::setVertex(lua_State* L)
{
    auto* self   = luax_checkmesh(L, 1);
    size_t index = (size_t)luaL_checkinteger(L, 2) - 1;
    bool isTable = lua_istable(L, 3);

    const auto& vertexFormat = self->getVertexFormat();

    char* data    = nullptr;
    size_t offset = 0;
    luax_catchexcept(L, [&]() { data = (char*)self->checkVertexDataOffset(index, &offset); });

    int tableIndex = isTable ? 1 : 3;

    if (isTable)
    {
        for (const auto& member : vertexFormat)
        {
            int components = member.info.components;

            for (int i = tableIndex; i < tableIndex + components; i++)
                lua_rawgeti(L, 3, i);

            luax_writebufferdata(L, -components, member.declaration.format, data + member.offset);

            tableIndex += components;
            lua_pop(L, components);
        }
    }
    else
    {
        for (const auto& member : vertexFormat)
        {
            luax_writebufferdata(L, tableIndex, member.declaration.format, data + member.offset);
            tableIndex += member.info.components;
        }
    }

    self->setVertexDataModified(offset, self->getVertexStride());
    return 0;
}

int Wrap_Mesh::getVertex(lua_State* L)
{
    auto* self   = luax_checkmesh(L, 1);
    size_t index = (size_t)luaL_checkinteger(L, 2) - 1;

    const auto& vertexFormat = self->getVertexFormat();

    const char* data = nullptr;
    luax_catchexcept(L, [&]() { data = (const char*)self->checkVertexDataOffset(index, nullptr); });

    int n = 0;
    for (const auto& member : vertexFormat)
    {
        luax_readbufferdata(L, member.declaration.format, data + member.offset);
        n += member.info.components;
    }

    return n;
}

int Wrap_Mesh::setVertexAttribute(lua_State* L)
{
    auto* self         = luax_checkmesh(L, 1);
    size_t vertexIndex = (size_t)luaL_checkinteger(L, 2) - 1;
    int attributeIndex = (int)luaL_checkinteger(L, 3) - 1;

    const auto& vertexFormat = self->getVertexFormat();

    if (attributeIndex < 0 || attributeIndex >= (int)vertexFormat.size())
        return luaL_error(L, "Invalid vertex attribute index: %d.", attributeIndex + 1);

    const auto& member = vertexFormat[attributeIndex];
    char* data         = nullptr;
    size_t offset      = 0;

    luax_catchexcept(L, [&]() { data = (char*)self->checkVertexDataOffset(vertexIndex, &offset); });

    luax_writebufferdata(L, 4, member.declaration.format, data + member.offset);
    self->setVertexDataModified(offset + member.offset, member.size);

    return 0;
}

int Wrap_Mesh::getVertexAttribute(lua_State* L)
{
    auto* self         = luax_checkmesh(L, 1);
    size_t vertexIndex = (size_t)luaL_checkinteger(L, 2) - 1;
    int attributeIndex = (int)luaL_checkinteger(L, 3) - 1;

    const auto& vertexFormat = self->getVertexFormat();

    if (attributeIndex < 0 || attributeIndex >= (int)vertexFormat.size())
        return luaL_error(L, "Invalid vertex attribute index: %d.", attributeIndex + 1);

    const auto& member = vertexFormat[attributeIndex];
    const char* data   = nullptr;
    luax_catchexcept(L, [&]() { data = (const char*)self->checkVertexDataOffset(vertexIndex, nullptr); });

    luax_readbufferdata(L, member.declaration.format, data + member.offset);

    return member.info.components;
}

int Wrap_Mesh::getVertexCount(lua_State* L)
{
    auto* self = luax_checkmesh(L, 1);
    lua_pushinteger(L, self->getVertexCount());

    return 1;
}

int Wrap_Mesh::getVertexFormat(lua_State* L)
{
    auto* self = luax_checkmesh(L, 1);

    const auto& vertexFormat = self->getVertexFormat();
    lua_createtable(L, (int)vertexFormat.size(), 0);

    std::string_view name {};

    for (size_t index = 0; index < vertexFormat.size(); index++)
    {
        const auto& member = vertexFormat[index];

        if (!getConstant(member.declaration.format, name))
            return luax_enumerror(L, "vertex attribute data type", BufferAttributes, name);

        lua_createtable(L, 0, 4);

        luax_pushstring(L, member.declaration.name.c_str());
        lua_setfield(L, -2, "name");

        lua_pushnumber(L, member.declaration.bindingLocation);
        lua_setfield(L, -2, "location");

        std::string_view formatstr = "unknown";
        getConstant(member.declaration.format, formatstr);
        luax_pushstring(L, formatstr);
        lua_setfield(L, -2, "format");

        lua_pushinteger(L, member.declaration.arrayLength);
        lua_setfield(L, -2, "arraylength");

        lua_pushinteger(L, member.offset);
        lua_setfield(L, -2, "offset");

        lua_rawseti(L, -2, (int)index + 1);
    }

    return 1;
}

int Wrap_Mesh::setAttributeEnabled(lua_State* L)
{
    auto* self  = luax_checkmesh(L, 1);
    bool enable = luax_checkboolean(L, 3);

    if (lua_type(L, 2) == LUA_TSTRING)
    {
        const char* name = luaL_checkstring(L, 2);
        luax_catchexcept(L, [&]() { self->setAttributeEnabled(name, enable); });
    }
    else
    {
        int location = (int)luaL_checkinteger(L, 2);
        luax_catchexcept(L, [&]() { self->setAttributeEnabled(location, enable); });
    }

    return 0;
}

int Wrap_Mesh::isAttributeEnabled(lua_State* L)
{
    auto* self   = luax_checkmesh(L, 1);
    bool enabled = false;

    if (lua_type(L, 2) == LUA_TSTRING)
    {
        const char* name = luaL_checkstring(L, 2);
        luax_catchexcept(L, [&]() { enabled = self->isAttributeEnabled(name); });
    }
    else
    {
        int location = (int)luaL_checkinteger(L, 2);
        luax_catchexcept(L, [&]() { enabled = self->isAttributeEnabled(location); });
    }

    luax_pushboolean(L, enabled);
    return 1;
}

int Wrap_Mesh::attachAttribute(lua_State* L)
{
    return 0;
}

int Wrap_Mesh::detachAttribute(lua_State* L)
{
    return 0;
}

int Wrap_Mesh::getAttachedAttributes(lua_State* L)
{
    auto* self             = luax_checkmesh(L, 1);
    const auto& attributes = self->getAttachedAttributes();

    lua_createtable(L, (int)attributes.size(), 0);

    for (int index = 0; index < (int)attributes.size(); index++)
    {
        const auto& attribute = attributes[index];

        lua_createtable(L, 0, 7);

        luax_pushstring(L, attribute.name);
        lua_setfield(L, -2, "name");

        lua_pushnumber(L, attribute.bindingLocation);
        lua_setfield(L, -2, "location");

        luax_pushtype(L, attribute.buffer.get());
        lua_setfield(L, -2, "buffer");

        std::string_view stepType {};
        if (!love::getConstant(attribute.step, stepType))
            return luaL_error(L, "Invalid vertex attribute step.");
        luax_pushstring(L, stepType);
        lua_setfield(L, -2, "step");

        const auto& member = attribute.buffer->getDataMember(attribute.indexInBuffer);
        luax_pushstring(L, member.declaration.name);
        lua_setfield(L, -2, "nameinbuffer");

        lua_pushnumber(L, member.declaration.bindingLocation);
        lua_setfield(L, -2, "locationinbuffer");

        lua_pushinteger(L, attribute.startArrayIndex + 1);
        lua_setfield(L, -2, "startindex");

        lua_rawseti(L, -1, index + 1);
    }

    return 1;
}

int Wrap_Mesh::getVertexBuffer(lua_State* L)
{
    auto* self = luax_checkmesh(L, 1);
    luax_pushtype(L, self->getVertexBuffer());

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
        luax_catchexcept(L, [&]() { self->setVertexMap(); });
        return 0;
    }

    if (luax_istype(L, 2, Data::type))
    {
        auto* data               = luax_totype<Data>(L, 2, Data::type);
        const char* indexTypeStr = luaL_checkstring(L, 3);
        IndexDataType type       = INDEX_MAX_ENUM;

        if (!getConstant(indexTypeStr, type))
            return luax_enumerror(L, "index data type", IndexDataTypes, indexTypeStr);

        size_t dataTypeSize = getIndexDataSize(type);
        int indexCount      = (int)luaL_optinteger(L, 4, data->getSize() / dataTypeSize);
        if (indexCount < 1 || indexCount * dataTypeSize > data->getSize())
            return luaL_error(L, "Invalid index count: %d", indexCount);

        luax_catchexcept(L, [&]() { self->setVertexMap(type, data->getData(), indexCount * dataTypeSize); });
        return 0;
    }

    bool isTable = lua_istable(L, 2);
    int nargs    = isTable ? (int)luax_objlen(L, 2) : lua_gettop(L) - 1;

    std::vector<uint32_t> map {};
    map.reserve(nargs);

    if (isTable)
    {
        for (int index = 0; index < nargs; index++)
        {
            lua_rawgeti(L, 2, index + 1);
            map.push_back(uint32_t(luaL_checkinteger(L, -1)) - 1);
            lua_pop(L, 1);
        }
    }
    else
    {
        for (int i = 0; i < nargs; i++)
            map.push_back(uint32_t(luaL_checkinteger(L, i + 2) - 1));
    }

    luax_catchexcept(L, [&]() { self->setVertexMap(map); });
    return 0;
}

int Wrap_Mesh::getVertexMap(lua_State* L)
{
    auto* self = luax_checkmesh(L, 1);

    std::vector<uint32_t> map {};
    bool hasVertexMap = false;
    luax_catchexcept(L, [&]() { hasVertexMap = self->getVertexMap(map); });

    if (!hasVertexMap)
    {
        lua_pushnil(L);
        return 1;
    }

    int nelements = (int)map.size();
    lua_createtable(L, nelements, 0);

    for (int i = 0; i < nelements; i++)
    {
        lua_pushinteger(L, lua_Integer(map[i]) + 1);
        lua_rawseti(L, -2, i + 1);
    }

    return 1;
}

int Wrap_Mesh::setIndexBuffer(lua_State* L)
{
    auto* self         = luax_checkmesh(L, 1);
    BufferBase* buffer = nullptr;

    if (!lua_isnoneornil(L, 2))
        buffer = luax_checkbuffer(L, 2);

    luax_catchexcept(L, [&]() { self->setIndexBuffer(buffer); });

    return 0;
}

int Wrap_Mesh::getIndexBuffer(lua_State* L)
{
    auto* self = luax_checkmesh(L, 1);
    luax_pushtype(L, self->getIndexBuffer());

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
        luax_catchexcept(L, [&]() { self->setTexture(texture); });
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
    auto* self         = luax_checkmesh(L, 1);
    const char* string = luaL_checkstring(L, 2);
    PrimitiveType mode = PRIMITIVE_MAX_ENUM;

    if (!getConstant(string, mode))
        return luax_enumerror(L, "mesh draw mode", PrimitiveTypes, string);

    self->setDrawMode(mode);
    return 0;
}

int Wrap_Mesh::getDrawMode(lua_State* L)
{
    auto* self = luax_checkmesh(L, 1);
    auto mode  = self->getDrawMode();
    std::string_view string {};

    if (!getConstant(mode, string))
        return luaL_error(L, "Unknown mesh draw mode.");

    luax_pushstring(L, string);
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
        luax_catchexcept(L, [&]() { self->setDrawRange(start, count); });
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
    { "setVertices",           Wrap_Mesh::setVertices           },
    { "setVertex",             Wrap_Mesh::setVertex             },
    { "getVertex",             Wrap_Mesh::getVertex             },
    { "setVertexAttribute",    Wrap_Mesh::setVertexAttribute    },
    { "getVertexAttribute",    Wrap_Mesh::getVertexAttribute    },
    { "getVertexCount",        Wrap_Mesh::getVertexCount        },
    { "getVertexFormat",       Wrap_Mesh::getVertexFormat       },
    { "setAttributeEnabled",   Wrap_Mesh::setAttributeEnabled   },
    { "isAttributeEnabled",    Wrap_Mesh::isAttributeEnabled    },
    { "attachAttribute",       Wrap_Mesh::attachAttribute       },
    { "detachAttribute",       Wrap_Mesh::detachAttribute       },
    { "getAttachedAttributes", Wrap_Mesh::getAttachedAttributes },
    { "getVertexBuffer",       Wrap_Mesh::getVertexBuffer       },
    { "flush",                 Wrap_Mesh::flush                 },
    { "setVertexMap",          Wrap_Mesh::setVertexMap          },
    { "getVertexMap",          Wrap_Mesh::getVertexMap          },
    { "setIndexBuffer",        Wrap_Mesh::setIndexBuffer        },
    { "getIndexBuffer",        Wrap_Mesh::getIndexBuffer        },
    { "setTexture",            Wrap_Mesh::setTexture            },
    { "getTexture",            Wrap_Mesh::getTexture            },
    { "setDrawMode",           Wrap_Mesh::setDrawMode           },
    { "getDrawMode",           Wrap_Mesh::getDrawMode           },
    { "setDrawRange",          Wrap_Mesh::setDrawRange          },
    { "getDrawRange",          Wrap_Mesh::getDrawRange          }
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
