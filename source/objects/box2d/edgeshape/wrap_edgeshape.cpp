#include "edgeshape/wrap_edgeshape.h"
#include "shape/wrap_shape.h"

using namespace love;

EdgeShape* Wrap_EdgeShape::CheckEdgeShape(lua_State* L, int index)
{
    return Luax::CheckType<EdgeShape>(L, index);
}

int Wrap_EdgeShape::SetNextVertex(lua_State* L)
{
    EdgeShape* self = Wrap_EdgeShape::CheckEdgeShape(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    self->SetNextVertex(x, y);

    return 0;
}

int Wrap_EdgeShape::SetPreviousVertex(lua_State* L)
{
    EdgeShape* self = Wrap_EdgeShape::CheckEdgeShape(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    self->SetPreviousVertex(x, y);

    return 0;
}

int Wrap_EdgeShape::GetNextVertex(lua_State* L)
{
    EdgeShape* self = Wrap_EdgeShape::CheckEdgeShape(L, 1);
    b2Vec2 vec      = self->GetNextVertex();

    lua_pushnumber(L, vec.x);
    lua_pushnumber(L, vec.y);

    return 2;
}

int Wrap_EdgeShape::GetPreviousVertex(lua_State* L)
{
    EdgeShape* self = Wrap_EdgeShape::CheckEdgeShape(L, 1);
    b2Vec2 vec      = self->GetPreviousVertex();

    lua_pushnumber(L, vec.x);
    lua_pushnumber(L, vec.y);

    return 2;
}

int Wrap_EdgeShape::GetPoints(lua_State* L)
{
    EdgeShape* self = Wrap_EdgeShape::CheckEdgeShape(L, 1);
    lua_remove(L, 1);

    return self->GetPoints(L);
}

int Wrap_EdgeShape::Register(lua_State* L)
{
    // clang-format off
    luaL_Reg funcs[] = {
        { "setNextVertex", SetNextVertex },
        { "setPreviousVertex", SetPreviousVertex },
        { "getNextVertex", GetNextVertex },
        { "getPreviousVertex", GetPreviousVertex },
        { "getPoints", GetPoints },
        { 0, 0 }
    };
    // clang-format on

    return Luax::RegisterType(L, &EdgeShape::type, Wrap_Shape::functions, funcs, nullptr);
}
