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

    Luax::PushBoolean(L, self->IsSensor());

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

    Luax::PushBoolean(L, self->TestPoint(x, y));

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

    Luax::PushBoolean(L, !fixture->IsValid());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "destroy",        Wrap_Fixture::Destroy        },
    { "getBody",        Wrap_Fixture::GetBody        },
    { "getBoundingBox", Wrap_Fixture::GetBoundingBox },
    { "getCategory",    Wrap_Fixture::GetCategory    },
    { "getDensity",     Wrap_Fixture::GetDensity     },
    { "getFilterData",  Wrap_Fixture::GetFilterData  },
    { "getFriction",    Wrap_Fixture::GetFriction    },
    { "getGroupIndex",  Wrap_Fixture::GetGroupIndex  },
    { "getMask",        Wrap_Fixture::GetMask        },
    { "getMassData",    Wrap_Fixture::GetMassData    },
    { "getRestitution", Wrap_Fixture::GetRestitution },
    { "getShape",       Wrap_Fixture::GetShape       },
    { "getType",        Wrap_Fixture::GetType        },
    { "getUserData",    Wrap_Fixture::GetUserdata    },
    { "isDestroyed",    Wrap_Fixture::IsDestroyed    },
    { "isSensor",       Wrap_Fixture::IsSensor       },
    { "rayCast",        Wrap_Fixture::RayCast        },
    { "setCategory",    Wrap_Fixture::SetCategory    },
    { "setDensity",     Wrap_Fixture::SetDensity     },
    { "setFilterData",  Wrap_Fixture::SetFilterData  },
    { "setFriction",    Wrap_Fixture::SetFriction    },
    { "setGroupIndex",  Wrap_Fixture::SetGroupIndex  },
    { "setMask",        Wrap_Fixture::SetMask        },
    { "setRestitution", Wrap_Fixture::SetRestitution },
    { "setSensor",      Wrap_Fixture::SetSensor      },
    { "setUserData",    Wrap_Fixture::SetUserdata    },
    { "testPoint",      Wrap_Fixture::TestPoint      },
    { 0,                0                       }
};
// clang-format on

int Wrap_Fixture::Register(lua_State* L)
{
    return Luax::RegisterType(L, &Fixture::type, functions, nullptr);
}
