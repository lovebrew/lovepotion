#include <objects/shape/types/chainshape/wrap_chainshape.hpp>

#include <modules/physics/physics.hpp>
#include <modules/physics/wrap_physics.hpp>

using namespace love;

ChainShape* Wrap_ChainShape::CheckChainShape(lua_State* L, int index)
{
    return luax::CheckType<ChainShape>(L, index);
}

int Wrap_ChainShape::SetNextVertex(lua_State* L)
{
    auto* self = Wrap_ChainShape::CheckChainShape(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    luax::CatchException(L, [&]() { self->SetNextVertex(x, y); });

    return 0;
}

int Wrap_ChainShape::SetPreviousVertex(lua_State* L)
{
    auto* self = Wrap_ChainShape::CheckChainShape(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    luax::CatchException(L, [&]() { self->SetPreviousVertex(x, y); });

    return 0;
}

int Wrap_ChainShape::GetVertexCount(lua_State* L)
{
    auto* self = Wrap_ChainShape::CheckChainShape(L, 1);

    int count = 0;
    luax::CatchException(L, [&]() { count = self->GetVertexCount(); });

    lua_pushinteger(L, count);

    return 1;
}

int Wrap_ChainShape::GetPoint(lua_State* L)
{
    auto* self = Wrap_ChainShape::CheckChainShape(L, 1);

    int index = luaL_checkinteger(L, 2) - 1;

    b2Vec2 point {};
    luax::CatchException(L, [&]() { point = self->GetPoint(index); });

    lua_pushnumber(L, point.x);
    lua_pushnumber(L, point.y);

    return 2;
}

int Wrap_ChainShape::GetNextVertex(lua_State* L)
{
    auto* self = Wrap_ChainShape::CheckChainShape(L, 1);

    b2Vec2 vertex {};
    luax::CatchException(L, [&]() { vertex = self->GetNextVertex(); });

    lua_pushnumber(L, vertex.x);
    lua_pushnumber(L, vertex.y);

    return 2;
}

int Wrap_ChainShape::GetPreviousVertex(lua_State* L)
{
    auto* self = Wrap_ChainShape::CheckChainShape(L, 1);

    b2Vec2 vertex {};
    luax::CatchException(L, [&]() { vertex = self->GetPreviousVertex(); });

    lua_pushnumber(L, vertex.x);
    lua_pushnumber(L, vertex.y);

    return 2;
}

int Wrap_ChainShape::GetPoints(lua_State* L)
{
    auto* self = Wrap_ChainShape::CheckChainShape(L, 1);

    const b2Vec2* points {};
    luax::CatchException(L, [&]() { points = self->GetPoints(); });

    const auto count = self->GetVertexCount();
    if (!lua_checkstack(L, count * 2))
        return luaL_error(L, "Too many return values");

    for (int i = 0; i < count; i++)
    {
        const auto point = Physics::ScaleUp(points[i]);

        lua_pushnumber(L, point.x);
        lua_pushnumber(L, point.y);
    }

    return count * 2;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "setNextVertex",     Wrap_ChainShape::SetNextVertex     },
    { "setPreviousVertex", Wrap_ChainShape::SetPreviousVertex },
    { "getVertexCount",    Wrap_ChainShape::GetVertexCount    },
    { "getPoint",          Wrap_ChainShape::GetPoint          },
    { "getNextVertex",     Wrap_ChainShape::GetNextVertex     },
    { "getPreviousVertex", Wrap_ChainShape::GetPreviousVertex },
    { "getPoints",         Wrap_ChainShape::GetPoints         }
};
// clang-format on

int Wrap_ChainShape::Register(lua_State* L)
{
    return luax::RegisterType(L, &ChainShape::type, Wrap_Shape::shapeFunctions, functions);
}
