#include "objects/box2d/shape/wrap_shape.h"

using namespace love;

Shape* Wrap_Shape::CheckShape(lua_State* L, int index)
{
    return Luax::CheckType<Shape>(L, index);
}

int Wrap_Shape::GetType(lua_State* L)
{
    Shape* self      = Wrap_Shape::CheckShape(L, 1);
    const char* type = "";

    Shape::GetConstant(self->GetType(), type);

    lua_pushstring(L, type);

    return 1;
}

int Wrap_Shape::GetRadius(lua_State* L)
{
    Shape* self = Wrap_Shape::CheckShape(L, 1);

    lua_pushnumber(L, self->GetRadius());

    return 1;
}

int Wrap_Shape::GetChildCount(lua_State* L)
{
    Shape* self = Wrap_Shape::CheckShape(L, 1);

    lua_pushinteger(L, self->GetChildCount());

    return 1;
}

int Wrap_Shape::TestPoint(lua_State* L)
{
    Shape* self = Wrap_Shape::CheckShape(L, 1);

    float x  = luaL_checknumber(L, 2);
    float y  = luaL_checknumber(L, 3);
    float r  = luaL_checknumber(L, 4);
    float px = luaL_checknumber(L, 5);
    float py = luaL_checknumber(L, 6);

    bool result = self->TestPoint(x, y, r, px, py);

    Luax::PushBoolean(L, result);

    return 1;
}

int Wrap_Shape::RayCast(lua_State* L)
{
    Shape* self = Wrap_Shape::CheckShape(L, 1);
    lua_remove(L, 1);

    int ret = 0;

    Luax::CatchException(L, [&]() { ret = self->RayCast(L); });

    return ret;
}

int Wrap_Shape::ComputeAABB(lua_State* L)
{
    Shape* self = Wrap_Shape::CheckShape(L, 1);
    lua_remove(L, 1);

    return self->ComputeAABB(L);
}

int Wrap_Shape::ComputeMass(lua_State* L)
{
    Shape* self = Wrap_Shape::CheckShape(L, 1);
    lua_remove(L, 1);

    return self->ComputeMass(L);
}

// clang-format off
const luaL_Reg Wrap_Shape::functions[8] =
{
    { "computeAABB",   Wrap_Shape::ComputeAABB   },
    { "computeMass",   Wrap_Shape::ComputeMass   },
    { "getChildCount", Wrap_Shape::GetChildCount },
    { "getRadius",     Wrap_Shape::GetRadius     },
    { "getType",       Wrap_Shape::GetType       },
    { "rayCast",       Wrap_Shape::RayCast       },
    { "testPoint",     Wrap_Shape::TestPoint     },
    { 0,               0                         }
};
// clang-format on

int Wrap_Shape::Register(lua_State* L)
{
    return Luax::RegisterType(L, &Shape::type, Wrap_Shape::functions, nullptr);
}
