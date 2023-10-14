#include <objects/shape/types/edgeshape/wrap_edgeshape.hpp>

using namespace love;

EdgeShape* Wrap_EdgeShape::CheckEdgeShape(lua_State* L, int index)
{
    return luax::CheckType<EdgeShape>(L, index);
}

int Wrap_EdgeShape::SetNextVertex(lua_State* L)
{
    auto* self = CheckEdgeShape(L, 1);

    float x = (float)luaL_checknumber(L, 2);
    float y = (float)luaL_checknumber(L, 3);

    luax::CatchException(L, [&]() { self->SetNextVertex(x, y); });

    return 0;
}

int Wrap_EdgeShape::SetPreviousVertex(lua_State* L)
{
    auto* self = CheckEdgeShape(L, 1);

    float x = (float)luaL_checknumber(L, 2);
    float y = (float)luaL_checknumber(L, 3);

    luax::CatchException(L, [&]() { self->SetPreviousVertex(x, y); });

    return 0;
}

int Wrap_EdgeShape::GetNextVertex(lua_State* L)
{
    auto* self = CheckEdgeShape(L, 1);

    b2Vec2 vertex {};

    luax::CatchException(L, [&]() { vertex = self->GetNextVertex(); });

    lua_pushnumber(L, vertex.x);
    lua_pushnumber(L, vertex.y);

    return 2;
}

int Wrap_EdgeShape::GetPreviousVertex(lua_State* L)
{
    auto* self = CheckEdgeShape(L, 1);

    b2Vec2 vertex {};

    luax::CatchException(L, [&]() { vertex = self->GetPreviousVertex(); });

    lua_pushnumber(L, vertex.x);
    lua_pushnumber(L, vertex.y);

    return 2;
}

int Wrap_EdgeShape::GetPoints(lua_State* L)
{
    auto* self = CheckEdgeShape(L, 1);
    lua_remove(L, 1);

    int count = 0;
    luax::CatchException(L, [&]() { count = self->GetPoints(L); });

    return count;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "setNextVertex",     Wrap_EdgeShape::SetNextVertex     },
    { "setPreviousVertex", Wrap_EdgeShape::SetPreviousVertex },
    { "getNextVertex",     Wrap_EdgeShape::GetNextVertex     },
    { "getPreviousVertex", Wrap_EdgeShape::GetPreviousVertex },
    { "getPoints",         Wrap_EdgeShape::GetPoints         }
};
// clang-format on

int Wrap_EdgeShape::Register(lua_State* L)
{
    return luax::RegisterType(L, &EdgeShape::type, Wrap_Shape::shapeFunctions, functions);
}
