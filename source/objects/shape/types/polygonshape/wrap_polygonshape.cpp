#include <objects/shape/types/polygonshape/wrap_polygonshape.hpp>

using namespace love;

PolygonShape* Wrap_PolygonShape::CheckPolygonShape(lua_State* L, int index)
{
    return luax::CheckType<PolygonShape>(L, index);
}

int Wrap_PolygonShape::GetPoints(lua_State* L)
{
    auto* self = Wrap_PolygonShape::CheckPolygonShape(L, 1);
    lua_remove(L, 1);

    int count = 0;
    luax::CatchException(L, [&]() { count = self->GetPoints(L); });

    return count;
}

int Wrap_PolygonShape::Validate(lua_State* L)
{
    auto* self = Wrap_PolygonShape::CheckPolygonShape(L, 1);

    bool valid = false;
    luax::CatchException(L, [&]() { valid = self->Validate(); });

    luax::PushBoolean(L, valid);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getPoints", Wrap_PolygonShape::GetPoints },
    { "validate",  Wrap_PolygonShape::Validate  }
};
// clang-format on

int Wrap_PolygonShape::Register(lua_State* L)
{
    return luax::RegisterType(L, &PolygonShape::type, Wrap_Shape::shapeFunctions, functions);
}
