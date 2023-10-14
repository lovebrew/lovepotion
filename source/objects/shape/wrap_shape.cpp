#include <objects/shape/wrap_shape.hpp>

using namespace love;

Shape* Wrap_Shape::CheckShape(lua_State* L, int index)
{
    return luax::CheckType<Shape>(L, index);
}

void Wrap_Shape::PushShape(lua_State* L, Shape* shape)
{
    if (shape == nullptr)
        return lua_pushnil(L);

    switch (shape->GetType())
    {
        default:
        {
            luax::PushType(L, shape);
            break;
        }
    }
}

int Wrap_Shape::GetType(lua_State* L)
{
    auto* self = Wrap_Shape::CheckShape(L, 1);

    auto type = Shape::shapeTypes.ReverseFind(self->GetType());
    lua_pushstring(L, *type);

    return 1;
}

int Wrap_Shape::GetRadius(lua_State* L)
{
    auto* self   = Wrap_Shape::CheckShape(L, 1);
    float radius = 0.0f;

    luax::CatchException(L, [&]() { radius = self->GetRadius(); });

    lua_pushnumber(L, radius);

    return 1;
}

int Wrap_Shape::GetChildCount(lua_State* L)
{
    auto* self = Wrap_Shape::CheckShape(L, 1);
    int count  = 0;

    luax::CatchException(L, [&]() { lua_pushinteger(L, self->GetChildCount()); });

    lua_pushinteger(L, count);

    return 1;
}

int Wrap_Shape::SetFriction(lua_State* L)
{
    auto* self     = Wrap_Shape::CheckShape(L, 1);
    float friction = luaL_checknumber(L, 2);

    luax::CatchException(L, [&]() { self->SetFriction(friction); });

    return 0;
}

int Wrap_Shape::SetRestitution(lua_State* L)
{
    auto* self        = Wrap_Shape::CheckShape(L, 1);
    float restitution = luaL_checknumber(L, 2);

    luax::CatchException(L, [&]() { self->SetRestitution(restitution); });

    return 0;
}

int Wrap_Shape::SetDensity(lua_State* L)
{
    auto* self    = Wrap_Shape::CheckShape(L, 1);
    float density = luaL_checknumber(L, 2);

    luax::CatchException(L, [&]() { self->SetDensity(density); });

    return 0;
}

int Wrap_Shape::SetSensor(lua_State* L)
{
    auto* self  = Wrap_Shape::CheckShape(L, 1);
    bool sensor = luax::CheckBoolean(L, 2);

    luax::CatchException(L, [&]() { self->SetSensor(sensor); });

    return 0;
}

int Wrap_Shape::GetFriction(lua_State* L)
{
    auto* self     = Wrap_Shape::CheckShape(L, 1);
    float friction = 0.0f;

    luax::CatchException(L, [&]() { friction = self->GetFriction(); });

    lua_pushnumber(L, friction);

    return 1;
}

int Wrap_Shape::GetRestitution(lua_State* L)
{
    auto* self        = Wrap_Shape::CheckShape(L, 1);
    float restitution = 0.0f;

    luax::CatchException(L, [&]() { restitution = self->GetRestitution(); });

    lua_pushnumber(L, restitution);

    return 1;
}

int Wrap_Shape::GetDensity(lua_State* L)
{
    auto* self    = Wrap_Shape::CheckShape(L, 1);
    float density = 0.0f;

    luax::CatchException(L, [&]() { density = self->GetDensity(); });

    lua_pushnumber(L, density);

    return 1;
}

int Wrap_Shape::IsSensor(lua_State* L)
{
    auto* self  = Wrap_Shape::CheckShape(L, 1);
    bool sensor = false;

    luax::CatchException(L, [&]() { sensor = self->IsSensor(); });

    luax::PushBoolean(L, sensor);

    return 1;
}

int Wrap_Shape::GetBody(lua_State* L)
{
    auto* self = Wrap_Shape::CheckShape(L, 1);
    Body* body = self->GetBody();

    if (body == nullptr)
        return 0;

    luax::PushType(L, body);

    return 1;
}

int Wrap_Shape::TestPoint(lua_State* L)
{
    auto* self = Wrap_Shape::CheckShape(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    bool hit = false;

    if (!lua_isnoneornil(L, 4))
    {
        float angle  = luaL_checknumber(L, 4);
        float pointX = luaL_checknumber(L, 5);
        float pointY = luaL_checknumber(L, 6);

        hit = luax::CatchException(L, [&]() { self->TestPoint(x, y, angle, pointX, pointY); });
    }
    else
        hit = luax::CatchException(L, [&]() { self->TestPoint(x, y); });

    luax::PushBoolean(L, hit);

    return 1;
}

int Wrap_Shape::RayCast(lua_State* L)
{
    auto* self = Wrap_Shape::CheckShape(L, 1);
    lua_remove(L, 1);

    int result = 0;
    luax::CatchException(L, [&]() { result = self->RayCast(L); });

    return result;
}

int Wrap_Shape::ComputeAABB(lua_State* L)
{
    auto* self = Wrap_Shape::CheckShape(L, 1);
    lua_remove(L, 1);

    int result = 0;
    luax::CatchException(L, [&]() { result = self->ComputeAABB(L); });

    return result;
}

int Wrap_Shape::ComputeMass(lua_State* L)
{
    auto* self = Wrap_Shape::CheckShape(L, 1);
    lua_remove(L, 1);

    int result = 0;
    luax::CatchException(L, [&]() { result = self->ComputeMass(L); });

    return result;
}

int Wrap_Shape::SetFilterData(lua_State* L)
{
    auto* self = Wrap_Shape::CheckShape(L, 1);

    int v[3] { 0 };
    v[0] = (int)luaL_checkinteger(L, 2);
    v[1] = (int)luaL_checkinteger(L, 3);
    v[2] = (int)luaL_checkinteger(L, 4);

    luax::CatchException(L, [&]() { self->SetFilterData(v); });

    return 0;
}

int Wrap_Shape::GetFilterData(lua_State* L)
{
    auto* self = Wrap_Shape::CheckShape(L, 1);

    int v[3] { 0 };
    luax::CatchException(L, [&]() { self->GetFilterData(v); });

    lua_pushinteger(L, v[0]);
    lua_pushinteger(L, v[1]);
    lua_pushinteger(L, v[2]);

    return 3;
}

int Wrap_Shape::SetCategory(lua_State* L)
{
    auto* self = Wrap_Shape::CheckShape(L, 1);
    lua_remove(L, 1);

    int result = 0;
    luax::CatchException(L, [&]() { result = self->SetCategory(L); });

    return result;
}

int Wrap_Shape::GetCategory(lua_State* L)
{
    auto* self = Wrap_Shape::CheckShape(L, 1);
    lua_remove(L, 1);

    int result = 0;
    luax::CatchException(L, [&]() { result = self->GetCategory(L); });

    return result;
}

int Wrap_Shape::SetMask(lua_State* L)
{
    auto* self = Wrap_Shape::CheckShape(L, 1);
    lua_remove(L, 1);

    int result = 0;
    luax::CatchException(L, [&]() { result = self->SetMask(L); });

    return result;
}

int Wrap_Shape::GetMask(lua_State* L)
{
    auto* self = Wrap_Shape::CheckShape(L, 1);
    lua_remove(L, 1);

    int result = 0;
    luax::CatchException(L, [&]() { result = self->GetMask(L); });

    return result;
}

int Wrap_Shape::SetUserdata(lua_State* L)
{
    auto* self = Wrap_Shape::CheckShape(L, 1);
    lua_remove(L, 1);

    int result = 0;
    luax::CatchException(L, [&]() { result = self->SetUserdata(L); });

    return result;
}

int Wrap_Shape::GetUserdata(lua_State* L)
{
    auto* self = Wrap_Shape::CheckShape(L, 1);
    lua_remove(L, 1);

    int result = 0;
    luax::CatchException(L, [&]() { result = self->GetUserdata(L); });

    return result;
}

int Wrap_Shape::GetBoundingBox(lua_State* L)
{
    auto* self = Wrap_Shape::CheckShape(L, 1);
    lua_remove(L, 1);

    int result = 0;
    luax::CatchException(L, [&]() { result = self->GetBoundingBox(L); });

    return result;
}

int Wrap_Shape::GetMassData(lua_State* L)
{
    auto* self = Wrap_Shape::CheckShape(L, 1);
    lua_remove(L, 1);

    int result = 0;
    luax::CatchException(L, [&]() { result = self->GetMassData(L); });

    return result;
}

int Wrap_Shape::GetGroupIndex(lua_State* L)
{
    auto* self = Wrap_Shape::CheckShape(L, 1);

    int index = 0;
    luax::CatchException(L, [&]() { index = self->GetGroupIndex(); });

    lua_pushinteger(L, index);

    return 1;
}

int Wrap_Shape::SetGroupIndex(lua_State* L)
{
    auto* self = Wrap_Shape::CheckShape(L, 1);
    int index  = luaL_checkinteger(L, 2);

    luax::CatchException(L, [&]() { self->SetGroupIndex(index); });

    return 0;
}

int Wrap_Shape::Destroy(lua_State* L)
{
    auto* self = Wrap_Shape::CheckShape(L, 1);

    luax::CatchException(L, [&]() { self->Destroy(); });

    return 0;
}

int Wrap_Shape::IsDestroyed(lua_State* L)
{
    auto* self = Wrap_Shape::CheckShape(L, 1);

    luax::PushBoolean(L, !self->IsValid());

    return 1;
}

// clang-format off
const luaL_Reg Wrap_Shape::shapeFunctions[0x1E] =
{
    { "getType",        Wrap_Shape::GetType        },
    { "getRadius",      Wrap_Shape::GetRadius      },
    { "getChildCount",  Wrap_Shape::GetChildCount  },
    { "setFriction",    Wrap_Shape::SetFriction    },
    { "setRestitution", Wrap_Shape::SetRestitution },
    { "setDensity",     Wrap_Shape::SetDensity     },
    { "setSensor",      Wrap_Shape::SetSensor      },
    { "getFriction",    Wrap_Shape::GetFriction    },
    { "getRestitution", Wrap_Shape::GetRestitution },
    { "getDensity",     Wrap_Shape::GetDensity     },
    { "isSensor",       Wrap_Shape::IsSensor       },
    { "getBody",        Wrap_Shape::GetBody        },
    { "testPoint",      Wrap_Shape::TestPoint      },
    { "rayCast",        Wrap_Shape::RayCast        },
    { "computeAABB",    Wrap_Shape::ComputeAABB    },
    { "computeMass",    Wrap_Shape::ComputeMass    },
    { "setFilterData",  Wrap_Shape::SetFilterData  },
    { "getFilterData",  Wrap_Shape::GetFilterData  },
    { "setCategory",    Wrap_Shape::SetCategory    },
    { "getCategory",    Wrap_Shape::GetCategory    },
    { "setMask",        Wrap_Shape::SetMask        },
    { "getMask",        Wrap_Shape::GetMask        },
    { "setUserdata",    Wrap_Shape::SetUserdata    },
    { "getUserdata",    Wrap_Shape::GetUserdata    },
    { "getBoundingBox", Wrap_Shape::GetBoundingBox },
    { "getMassData",    Wrap_Shape::GetMassData    },
    { "getGroupIndex",  Wrap_Shape::GetGroupIndex  },
    { "setGroupIndex",  Wrap_Shape::SetGroupIndex  },
    { "destroy",        Wrap_Shape::Destroy        },
    { "isDestroyed",    Wrap_Shape::IsDestroyed    }
};
// clang-format on

int Wrap_Shape::Register(lua_State* L)
{
    return luax::RegisterType(L, &Shape::type, Wrap_Shape::shapeFunctions);
}
