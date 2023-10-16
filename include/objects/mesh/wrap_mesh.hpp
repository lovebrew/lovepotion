#pragma once

#include <common/luax.hpp>
#include <objects/mesh/mesh.hpp>

namespace Wrap_Mesh
{
    love::Mesh* CheckMesh(lua_State* L, int index);

    int SetVertices(lua_State* L);

    int SetVertex(lua_State* L);

    int GetVertex(lua_State* L);

    int GetVertexCount(lua_State* L);

    int GetVertexFormat(lua_State* L);

    int GetVertexBuffer(lua_State*L);

    int Flush(lua_State* L);

    int SetVertexMap(lua_State* L);

    int GetVertexMap(lua_State* L);

    int SetIndexBuffer(lua_State* L);

    int GetIndexBuffer(lua_State* L);

    int SetTexture(lua_State* L);

    int GetTexture(lua_State* L);

    int SetDrawMode(lua_State*L);

    int GetDrawMode(lua_State*L);

    int SetDrawRange(lua_State*L);

    int GetDrawRange(lua_State*L);

    int Register(lua_State* L);
} // namespace Wrap_Mesh