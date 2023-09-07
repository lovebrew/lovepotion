#include <objects/mesh/wrap_mesh.hpp>

#include <objects/texture/wrap_texture.hpp>

#include <algorithm>

using namespace love;

Mesh* Wrap_Mesh::CheckMesh(lua_State* L, int index)
{
    return luax::CheckType<Mesh>(L, index);
}

int Wrap_Mesh::GetVertexCount(lua_State* L)
{
    auto* self = Wrap_Mesh::CheckMesh(L, 1);

    lua_pushinteger(L, self->GetVertexCount());

    return 1;
}

int Wrap_Mesh::Flush(lua_State* L)
{
    auto* self = Wrap_Mesh::CheckMesh(L, 1);

    self->Flush();

    return 0;
}

int Wrap_Mesh::GetVertexMap(lua_State* L)
{
    auto* self = Wrap_Mesh::CheckMesh(L, 1);

    std::vector<uint32_t> map {};
    bool hasVertexMap = false;

    luax::CatchException(L, [&]() { hasVertexMap = self->GetVertexMap(map); });

    if (!hasVertexMap)
    {
        lua_pushnil(L);
        return 1;
    }

    int count = (int)map.size();
    lua_createtable(L, count, 0);

    for (int index = 0; index < count; index++)
    {
        lua_pushinteger(L, lua_Integer(map[index]) + 1);
        lua_rawseti(L, -2, index + 1);
    }

    return 1;
}

int Wrap_Mesh::SetTexture(lua_State* L)
{
    auto* self = Wrap_Mesh::CheckMesh(L, 1);

    if (lua_isnoneornil(L, 2))
        self->SetTexture();
    else
    {
        auto* texture = Wrap_Texture::CheckTexture(L, 2);
        luax::CatchException(L, [&]() { self->SetTexture(texture); });
    }

    return 0;
}

int Wrap_Mesh::GetTexture(lua_State* L)
{
    auto* self    = Wrap_Mesh::CheckMesh(L, 1);
    auto* texture = self->GetTexture();

    if (texture == nullptr)
        return 0;

    luax::PushType(L, texture);
    return 1;
}

int Wrap_Mesh::SetDrawMode(lua_State* L)
{
    auto* self       = Wrap_Mesh::CheckMesh(L, 1);
    const char* name = luaL_checkstring(L, 2);

    std::optional<PrimitiveType> mode;

    if (!(mode = vertex::primitiveTypes.Find(name)))
        return luax::EnumError(L, "mesh draw mode", vertex::primitiveTypes, name);

    self->SetDrawMode(*mode);
    return 0;
}

int Wrap_Mesh::GetDrawMode(lua_State* L)
{
    auto* self = Wrap_Mesh::CheckMesh(L, 1);
    auto mode  = self->GetDrawMode();

    std::optional<const char*> name;
    if (!(name = vertex::primitiveTypes.ReverseFind(mode)))
        return luaL_error(L, "Unknown mesh draw mode.");

    lua_pushstring(L, *name);
    return 1;
}

int Wrap_Mesh::SetDrawRange(lua_State* L)
{
    auto* self = Wrap_Mesh::CheckMesh(L, 1);

    if (lua_isnoneornil(L, 2))
        self->SetDrawRange();
    else
    {
        int start = luaL_checkinteger(L, 2) - 1;
        int count = luaL_checkinteger(L, 3);

        luax::CatchException(L, [&]() { self->SetDrawRange(start, count); });
    }

    return 0;
}

int Wrap_Mesh::GetDrawRange(lua_State* L)
{
    auto* self = Wrap_Mesh::CheckMesh(L, 1);

    int start = 0;
    int count = 1;

    if (!self->GetDrawRange(start, count))
        return 0;

    lua_pushinteger(L, start + 1);
    lua_pushinteger(L, count);

    return 2;
}

// clang-format off
static constexpr luaL_Reg functions[] 
{
    { "getVertexCount", Wrap_Mesh::GetVertexCount },
    { "flush",          Wrap_Mesh::Flush },
    { "getVertexMap",   Wrap_Mesh::GetVertexMap },
    { "setTexture",     Wrap_Mesh::SetTexture },
    { "getTexture",     Wrap_Mesh::GetTexture },
    { "setDrawMode",    Wrap_Mesh::SetDrawMode },
    { "getDrawMode",    Wrap_Mesh::GetDrawMode },
    { "setDrawRange",   Wrap_Mesh::SetDrawRange },
    { "getDrawRange",   Wrap_Mesh::GetDrawRange },
};
// clang-format on

int Wrap_Mesh::Register(lua_State* L)
{
    return luax::RegisterType(L, &Mesh::type, functions);
}