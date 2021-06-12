#include "polygonshape/wrap_polygonshape.h"
#include "shape/wrap_shape.h"

using namespace love;

PolygonShape* Wrap_PolygonShape::CheckPolygonShape(lua_State* L, int index)
{
    return Luax::CheckType<PolygonShape>(L, index);
}

int Wrap_PolygonShape::GetPoints(lua_State* L)
{
    PolygonShape* self = Wrap_PolygonShape::CheckPolygonShape(L, 1);
    lua_remove(L, 1);

    return self->GetPoints(L);
}

int Wrap_PolygonShape::Validate(lua_State* L)
{
    PolygonShape* self = Wrap_PolygonShape::CheckPolygonShape(L, 1);

    Luax::PushBoolean(L, self->Validate());

    return 1;
}

int Wrap_PolygonShape::Register(lua_State* L)
{
    luaL_Reg funcs[] = { { "getPoints", GetPoints }, { "validate", Validate }, { 0, 0 } };

    return Luax::RegisterType(L, &PolygonShape::type, Wrap_Shape::functions, funcs, nullptr);
}
