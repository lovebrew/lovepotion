#include <objects/shape/types/circleshape/wrap_circleshape.hpp>

#include <modules/physics/physics.hpp>
#include <modules/physics/wrap_physics.hpp>

using namespace love;

CircleShape* Wrap_CircleShape::CheckCircleShape(lua_State* L, int index)
{
    return luax::CheckType<CircleShape>(L, index);
}

int Wrap_CircleShape::GetRadius(lua_State* L)
{
    auto* self = Wrap_CircleShape::CheckCircleShape(L, 1);

    float radius = 0.0f;
    luax::CatchException(L, [&]() { radius = self->GetRadius(); });

    lua_pushnumber(L, radius);

    return 1;
}

int Wrap_CircleShape::SetRadius(lua_State* L)
{
    auto* self = Wrap_CircleShape::CheckCircleShape(L, 1);

    float radius = (float)luaL_checknumber(L, 2);
    luax::CatchException(L, [&]() { self->SetRadius(radius); });

    return 0;
}

int Wrap_CircleShape::GetPoint(lua_State* L)
{
    auto* self = Wrap_CircleShape::CheckCircleShape(L, 1);

    float x, y = 0.0f;
    luax::CatchException(L, [&]() { self->GetPoint(x, y); });

    lua_pushnumber(L, x);
    lua_pushnumber(L, y);

    return 2;
}

int Wrap_CircleShape::SetPoint(lua_State* L)
{
    auto* self = Wrap_CircleShape::CheckCircleShape(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    self->SetPoint(x, y);

    return 0;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getRadius", Wrap_CircleShape::GetRadius },
    { "setRadius", Wrap_CircleShape::SetRadius },
    { "getPoint",  Wrap_CircleShape::GetPoint  },
    { "setPoint",  Wrap_CircleShape::SetPoint  }
};
// clang-format on

int Wrap_CircleShape::Register(lua_State* L)
{
    return luax::RegisterType(L, &CircleShape::type, Wrap_Shape::shapeFunctions, functions);
}
