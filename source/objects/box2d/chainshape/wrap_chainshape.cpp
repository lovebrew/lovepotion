#include "chainshape/wrap_chainshape.h"

#include "modules/physics/physics.h"
#include "shape/wrap_shape.h"

using namespace love;

ChainShape* Wrap_ChainShape::CheckChainShape(lua_State* L, int index)
{
    return Luax::CheckType<ChainShape>(L, index);
}

int Wrap_ChainShape::SetNextVertex(lua_State* L)
{
    ChainShape* self = Wrap_ChainShape::CheckChainShape(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    Luax::CatchException(L, [&]() { self->SetNextVertex(x, y); });

    return 0;
}

int Wrap_ChainShape::SetPreviousVertex(lua_State* L)
{
    ChainShape* self = Wrap_ChainShape::CheckChainShape(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    Luax::CatchException(L, [&]() { self->SetPreviousVertex(x, y); });

    return 0;
}

int Wrap_ChainShape::GetChildEdge(lua_State* L)
{
    ChainShape* self = Wrap_ChainShape::CheckChainShape(L, 1);
    int index        = luaL_checkinteger(L, 2) - 1;

    EdgeShape* edgeShape = nullptr;

    Luax::CatchException(L, [&]() { edgeShape = self->GetChildEdge(index); });

    Luax::PushType(L, edgeShape);
    edgeShape->Release();

    return 1;
}

int Wrap_ChainShape::GetVertexCount(lua_State* L)
{
    ChainShape* self = Wrap_ChainShape::CheckChainShape(L, 1);

    int count = self->GetVertexCount();
    lua_pushinteger(L, count);

    return 1;
}

int Wrap_ChainShape::GetPoint(lua_State* L)
{
    ChainShape* self = Wrap_ChainShape::CheckChainShape(L, 1);
    int index        = luaL_checkinteger(L, 2) - 1;

    b2Vec2 vec;

    Luax::CatchException(L, [&]() { vec = self->GetPoint(index); });

    lua_pushnumber(L, vec.x);
    lua_pushnumber(L, vec.y);

    return 2;
}

int Wrap_ChainShape::GetNextVertex(lua_State* L)
{
    ChainShape* self = Wrap_ChainShape::CheckChainShape(L, 1);
    b2Vec2 vec       = self->GetNextVertex();

    lua_pushnumber(L, vec.x);
    lua_pushnumber(L, vec.y);

    return 2;
}

int Wrap_ChainShape::GetPreviousVertex(lua_State* L)
{
    ChainShape* self = Wrap_ChainShape::CheckChainShape(L, 1);
    b2Vec2 vec       = self->GetPreviousVertex();

    lua_pushnumber(L, vec.x);
    lua_pushnumber(L, vec.y);

    return 2;
}

int Wrap_ChainShape::GetPoints(lua_State* L)
{
    ChainShape* self    = Wrap_ChainShape::CheckChainShape(L, 1);
    const b2Vec2* verts = self->GetPoints();

    int count = self->GetVertexCount();

    if (!lua_checkstack(L, count * 2))
        return luaL_error(L, "Too many return values!");

    for (int index = 0; index < count; index++)
    {
        b2Vec2 vec = Physics::ScaleUp(verts[index]);

        lua_pushnumber(L, vec.x);
        lua_pushnumber(L, vec.y);
    }

    return count * 2;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getChildEdge",      Wrap_ChainShape::GetChildEdge      },
    { "getNextVertex",     Wrap_ChainShape::GetNextVertex     },
    { "getPoint",          Wrap_ChainShape::GetPoint          },
    { "getPoints",         Wrap_ChainShape::GetPoints         },
    { "getPreviousVertex", Wrap_ChainShape::GetPreviousVertex },
    { "getVertexCount",    Wrap_ChainShape::GetVertexCount    },
    { "setNextVertex",     Wrap_ChainShape::SetNextVertex     },
    { "setPreviousVertex", Wrap_ChainShape::SetPreviousVertex },
    { 0,                   0                                  }
};
// clang-format on

int Wrap_ChainShape::Register(lua_State* L)
{
    return Luax::RegisterType(L, &ChainShape::type, Wrap_Shape::functions, functions, nullptr);
}
