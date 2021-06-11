#include "objects/box2d/fixture/wrap_fixture.h"

#include "body.h"
#include "shape.h"

using namespace love;

Fixture* Wrap_Fixture::CheckFixture(lua_State* L, int index)
{
    Fixture* self = Luax::CheckType<Fixture>(L, index);
    if (!self->IsValid())
        luaL_error(L, "Attempt to use destroyed fixture!");

    return self;
}

int Wrap_Fixture::GetType(lua_State* L)
{
    Fixture* self    = Wrap_Fixture::CheckFixture(L, 1);
    const char* type = "";

    Shape::GetConstant(self->GetType(), type);

    lua_pushstring(L, type);

    return 1;
}

int Wrap_Fixture::SetFriction(lua_State* L)
{
    Fixture* self  = Wrap_Fixture::CheckFixture(L, 1);
    float friction = luaL_checknumber(L, 2);

    self->SetFriction(friction);

    return 0;
}

int Wrap_Fixture::SetRestitution(lua_State* L)
{
    Fixture* self     = Wrap_Fixture::CheckFixture(L, 1);
    float restitution = luaL_checknumber(L, 2);

    self->SetRestitution(restitution);

    return 0;
}

int Wrap_Fixture::SetDensity(lua_State* L)
{
    Fixture* self = Wrap_Fixture::CheckFixture(L, 1);
    float density = luaL_checknumber(L, 2);

    Luax::CatchException(L, [&]() { self->SetDensity(density); });

    return 0;
}

int Wrap_Fixture::SetSensor(lua_State* L)
{
    Fixture* self  = Wrap_Fixture::CheckFixture(L, 1);
    bool setSensor = Luax::CheckBoolean(L, 2);

    self->SetSensor(setSensor);

    return 0;
}

int Wrap_Fixture::GetFriction(lua_State* L)
{
    Fixture* self = Wrap_Fixture::CheckFixture(L, 1);

    lua_pushnumber(L, self->GetFriction());

    return 1;
}

int Wrap_Fixture::GetRestitution(lua_State* L)
{
    Fixture* self = Wrap_Fixture::CheckFixture(L, 1);

    lua_pushnumber(L, self->GetRestitution());

    return 1;
}

int Wrap_Fixture::GetDensity(lua_State* L)
{
    Fixture* self = Wrap_Fixture::CheckFixture(L, 1);

    lua_pushnumber(L, self->GetDensity());

    return 1;
}

int Wrap_Fixture::IsSensor(lua_State* L)
{
    Fixture* self = Wrap_Fixture::CheckFixture(L, 1);

    lua_pushboolean(L, self->IsSensor());

    return 1;
}

int Wrap_Fixture::GetBody(lua_State* L)
{
    Fixture* self = Wrap_Fixture::CheckFixture(L, 1);
    Body* body    = self->GetBody();

    if (body == nullptr)
        return 0;

    Luax::PushType(L, body);

    return 1;
}

int Wrap_Fixture::GetShape(lua_State* L)
{
    Fixture* self = Wrap_Fixture::CheckFixture(L, 1);
    Shape* shape  = self->GetShape();

    if (shape == nullptr)
        return 0;

    switch (shape->GetType())
    {
        case Shape::SHAPE_EDGE:
            break;
        case Shape::SHAPE_CHAIN:
            break;
        case Shape::SHAPE_CIRCLE:
            break;
        case Shape::SHAPE_POLYGON:
            break;
        default:
            Luax::PushType(L, shape);
    }

    return 1;
}

int Wrap_Fixture::TestPoint(lua_State* L)
{
    Fixture* self = Wrap_Fixture::CheckFixture(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);
    ;

    lua_pushboolean(L, self->TestPoint(x, y));

    return 1;
}

int Wrap_Fixture::RayCast(lua_State* L)
{
    Fixture* self = Wrap_Fixture::CheckFixture(L, 1);
    lua_remove(L, 1);

    int ret = 0;
    Luax::CatchException(L, [&]() { ret = self->RayCast(L); });

    return ret;
}

int Wrap_Fixture::SetFilterData(lua_State* L)
{
    Fixture* self = Wrap_Fixture::CheckFixture(L, 1);

    int filter[3];

    filter[0] = luaL_checkinteger(L, 2);
    filter[1] = luaL_checkinteger(L, 3);
    filter[2] = luaL_checkinteger(L, 4);

    self->SetFilterData(filter);

    return 0;
}

int Wrap_Fixture::GetFilterData(lua_State* L)
{
    Fixture* self = Wrap_Fixture::CheckFixture(L, 1);

    int filter[3];
    self->GetFilterData(filter);

    lua_pushinteger(L, filter[0]);
    lua_pushinteger(L, filter[1]);
    lua_pushinteger(L, filter[2]);

    return 3;
}

int Wrap_Fixture::SetCategory(lua_State* L)
{
    Fixture* self = Wrap_Fixture::CheckFixture(L, 1);
    lua_remove(L, 1);

    return self->SetCategory(L);
}

int Wrap_Fixture::SetMask(lua_State* L)
{
    Fixture* self = Wrap_Fixture::CheckFixture(L, 1);
    lua_remove(L, 1);

    return self->SetMask(L);
}

int Wrap_Fixture::GetMask(lua_State* L)
{
    Fixture* self = Wrap_Fixture::CheckFixture(L, 1);
    lua_remove(L, 1);

    return self->GetMask(L);
}

int Wrap_Fixture::SetUserdata(lua_State* L)
{
    Fixture* self = Wrap_Fixture::CheckFixture(L, 1);
    lua_remove(L, 1);

    return self->SetUserdata(L);
}

int Wrap_Fixture::GetUserdata(lua_State* L)
{
    Fixture* self = Wrap_Fixture::CheckFixture(L, 1);
    lua_remove(L, 1);

    return self->GetUserdata(L);
}

int Wrap_Fixture::GetBoundingBox(lua_State* L)
{
    Fixture* self = Wrap_Fixture::CheckFixture(L, 1);
    lua_remove(L, 1);

    return self->GetBoundingBox(L);
}

int Wrap_Fixture::GetMassData(lua_State* L)
{
    Fixture* self = Wrap_Fixture::CheckFixture(L, 1);
    lua_remove(L, 1);

    return self->GetMassData(L);
}

int Wrap_Fixture::GetGroupIndex(lua_State* L)
{
    Fixture* self = Wrap_Fixture::CheckFixture(L, 1);
    int index     = self->GetGroupIndex();

    lua_pushinteger(L, index);

    return 1;
}

int Wrap_Fixture::SetGroupIndex(lua_State* L)
{
    Fixture* self = Wrap_Fixture::CheckFixture(L, 1);
    int index     = luaL_checkinteger(L, 2);

    self->SetGroupIndex(index);

    return 0;
}

int Wrap_Fixture::Destroy(lua_State* L)
{
    Fixture* self = Wrap_Fixture::CheckFixture(L, 1);

    Luax::CatchException(L, [&]() { self->Destroy(); });

    return 0;
}

int Wrap_Fixture::GetCategory(lua_State* L)
{
    Fixture* self = Wrap_Fixture::CheckFixture(L, 1);
    lua_remove(L, 1);

    return self->GetCategory(L);
}

int Wrap_Fixture::IsDestroyed(lua_State* L)
{
    Fixture* fixture = Luax::CheckType<Fixture>(L, 1);

    lua_pushboolean(L, !fixture->IsValid());

    return 1;
}

int Wrap_Fixture::Register(lua_State* L)
{
    luaL_Reg funcs[] = { { "getType", GetType },
                         { "setFriction", SetFriction },
                         { "setRestitution", SetRestitution },
                         { "setDensity", SetDensity },
                         { "setSensor", SetSensor },
                         { "getFriction", GetFriction },
                         { "getRestitution", GetRestitution },
                         { "getDensity", GetDensity },
                         { "getBody", GetBody },
                         { "getShape", GetShape },
                         { "isSensor", IsSensor },
                         { "testPoint", TestPoint },
                         { "rayCast", RayCast },
                         { "setFilterData", SetFilterData },
                         { "getFilterData", GetFilterData },
                         { "setCategory", SetCategory },
                         { "getCategory", GetCategory },
                         { "setMask", SetMask },
                         { "getMask", GetMask },
                         { "setUserData", SetUserdata },
                         { "getUserData", GetUserdata },
                         { "getBoundingBox", GetBoundingBox },
                         { "getMassData", GetMassData },
                         { "getGroupIndex", GetGroupIndex },
                         { "setGroupIndex", SetGroupIndex },
                         { "destroy", Destroy },
                         { "isDestroyed", IsDestroyed },
                         { 0, 0 } };

    return Luax::RegisterType(L, &Fixture::type, funcs, nullptr);
}
