#pragma once

#include "common/luax.hpp"
#include "modules/graphics/Mesh.hpp"

namespace love
{
    Mesh* luax_checkmesh(lua_State* L, int index);

    int open_mesh(lua_State* L);
} // namespace love

namespace Wrap_Mesh
{
    int setVertices(lua_State* L);

    int setVertex(lua_State* L);

    int getVertex(lua_State* L);

    int getVertexCount(lua_State* L);

    int flush(lua_State* L);

    int setVertexMap(lua_State* L);

    int getVertexMap(lua_State* L);

    int setIndexBuffer(lua_State* L);

    int getIndexBuffer(lua_State* L);

    int setTexture(lua_State* L);

    int getTexture(lua_State* L);

    int setDrawMode(lua_State* L);

    int getDrawMode(lua_State* L);

    int setDrawRange(lua_State* L);

    int getDrawRange(lua_State* L);

} // namespace Wrap_Mesh
