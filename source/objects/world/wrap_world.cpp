#include <objects/world/wrap_world.hpp>

using namespace love;

World* Wrap_World::CheckWorld(lua_State* L, int index)
{
    auto* world = luax::CheckType<World>(L, index);

    if (!world->IsValid())
        luaL_error(L, "Attempt to use destroyed world.");

    return world;
}

int Wrap_World::Update(lua_State* L)
{
    auto* self = Wrap_World::CheckWorld(L, 1);

    float delta = luaL_checknumber(L, 2);
    self->SetCallbacksL(L);

    if (lua_isnoneornil(L, 3))
        luax::CatchException(L, [&]() { self->Update(delta); });
    else
    {
        int velocityIterations = luaL_checkinteger(L, 3);
        int positionIterations = luaL_checkinteger(L, 4);

        luax::CatchException(
            L, [&]() { self->Update(delta, velocityIterations, positionIterations); });
    }

    return 0;
}

int Wrap_World::SetCallbacks(lua_State* L)
{
    auto* self = Wrap_World::CheckWorld(L, 1);
    lua_remove(L, 1);

    return self->SetCallbacks(L);
}

int Wrap_World::GetCallbacks(lua_State* L)
{
    auto* self = Wrap_World::CheckWorld(L, 1);
    lua_remove(L, 1);

    return self->GetCallbacks(L);
}

int Wrap_World::SetContactFilter(lua_State* L)
{
    auto* self = Wrap_World::CheckWorld(L, 1);
    lua_remove(L, 1);

    return self->SetContactFilter(L);
}

int Wrap_World::GetContactFilter(lua_State* L)
{
    auto* self = Wrap_World::CheckWorld(L, 1);
    lua_remove(L, 1);

    return self->GetContactFilter(L);
}

int Wrap_World::SetGravity(lua_State* L)
{
    auto* self = Wrap_World::CheckWorld(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    self->SetGravity(x, y);

    return 0;
}

int Wrap_World::GetGravity(lua_State* L)
{
    auto* self = Wrap_World::CheckWorld(L, 1);
    lua_remove(L, 1);

    return self->GetGravity(L);
}

int Wrap_World::TranslateOrigin(lua_State* L)
{
    auto* self = Wrap_World::CheckWorld(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    luax::CatchException(L, [&]() { self->TranslateOrigin(x, y); });

    return 0;
}

int Wrap_World::SetSleepingAllowed(lua_State* L)
{
    auto* self = Wrap_World::CheckWorld(L, 1);

    bool allowed = luax::CheckBoolean(L, 2);
    self->SetSleepingAllowed(allowed);

    return 0;
}

int Wrap_World::IsSleepingAllowed(lua_State* L)
{
    auto* self = Wrap_World::CheckWorld(L, 1);

    luax::PushBoolean(L, self->IsSleepingAllowed());

    return 1;
}

int Wrap_World::IsLocked(lua_State* L)
{
    auto* self = Wrap_World::CheckWorld(L, 1);

    luax::PushBoolean(L, self->IsLocked());

    return 1;
}

int Wrap_World::GetBodyCount(lua_State* L)
{
    auto* self = Wrap_World::CheckWorld(L, 1);

    lua_pushinteger(L, self->GetBodyCount());

    return 1;
}

int Wrap_World::GetJointCount(lua_State* L)
{
    auto* self = Wrap_World::CheckWorld(L, 1);

    lua_pushinteger(L, self->GetJointCount());

    return 1;
}

int Wrap_World::GetContactCount(lua_State* L)
{
    auto* self = Wrap_World::CheckWorld(L, 1);

    lua_pushinteger(L, self->GetContactCount());

    return 1;
}

int Wrap_World::GetBodies(lua_State* L)
{
    auto* self = Wrap_World::CheckWorld(L, 1);
    lua_remove(L, 1);

    int count = 0;
    luax::CatchException(L, [&]() { count = self->GetBodies(L); });

    return count;
}

int Wrap_World::GetJoints(lua_State* L)
{
    auto* self = Wrap_World::CheckWorld(L, 1);
    lua_remove(L, 1);

    int count = 0;
    luax::CatchException(L, [&]() { count = self->GetJoints(L); });

    return count;
}

int Wrap_World::GetContacts(lua_State* L)
{
    auto* self = Wrap_World::CheckWorld(L, 1);
    lua_remove(L, 1);

    int count = 0;
    luax::CatchException(L, [&]() { count = self->GetContacts(L); });

    return count;
}

int Wrap_World::QueryShapesInArea(lua_State* L)
{
    auto* self = Wrap_World::CheckWorld(L, 1);
    lua_remove(L, 1);

    return self->QueryShapesInArea(L);
}

int Wrap_World::GetShapesInArea(lua_State* L)
{
    auto* self = Wrap_World::CheckWorld(L, 1);
    lua_remove(L, 1);

    int count = 0;
    luax::CatchException(L, [&]() { count = self->GetShapesInArea(L); });

    return count;
}

int Wrap_World::RayCast(lua_State* L)
{
    auto* self = Wrap_World::CheckWorld(L, 1);
    lua_remove(L, 1);

    int count = 0;
    luax::CatchException(L, [&]() { count = self->RayCast(L); });

    return count;
}

int Wrap_World::RayCastAny(lua_State* L)
{
    auto* self = Wrap_World::CheckWorld(L, 1);
    lua_remove(L, 1);

    int count = 0;
    luax::CatchException(L, [&]() { count = self->RayCastAny(L); });

    return count;
}

int Wrap_World::RayCastClosest(lua_State* L)
{
    auto* self = Wrap_World::CheckWorld(L, 1);
    lua_remove(L, 1);

    int count = 0;
    luax::CatchException(L, [&]() { count = self->RayCastClosest(L); });

    return count;
}

int Wrap_World::Destroy(lua_State* L)
{
    auto* self = Wrap_World::CheckWorld(L, 1);

    luax::CatchException(L, [&]() { self->Destroy(); });

    return 0;
}

int Wrap_World::IsDestroyed(lua_State* L)
{
    auto* self = luax::CheckType<World>(L, 1);

    luax::PushBoolean(L, !self->IsValid());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "update",             Wrap_World::Update             },
    { "setCallbacks",       Wrap_World::SetCallbacks       },
    { "getCallbacks",       Wrap_World::GetCallbacks       },
    { "setContactFilter",   Wrap_World::SetContactFilter   },
    { "getContactFilter",   Wrap_World::GetContactFilter   },
    { "setGravity",         Wrap_World::SetGravity         },
    { "getGravity",         Wrap_World::GetGravity         },
    { "translateOrigin",    Wrap_World::TranslateOrigin    },
    { "setSleepingAllowed", Wrap_World::SetSleepingAllowed },
    { "isSleepingAllowed",  Wrap_World::IsSleepingAllowed  },
    { "isLocked",           Wrap_World::IsLocked           },
    { "getBodyCount",       Wrap_World::GetBodyCount       },
    { "getJointCount",      Wrap_World::GetJointCount      },
    { "getContactCount",    Wrap_World::GetContactCount    },
    { "getBodies",          Wrap_World::GetBodies          },
    { "getJoints",          Wrap_World::GetJoints          },
    { "getContacts",        Wrap_World::GetContacts        },
    { "queryShapesInArea",  Wrap_World::QueryShapesInArea  },
    { "getShapesInArea",    Wrap_World::GetShapesInArea    },
    { "rayCast",            Wrap_World::RayCast            },
    { "rayCastAny",         Wrap_World::RayCastAny         },
    { "rayCastClosest",     Wrap_World::RayCastClosest     },
    { "destroy",            Wrap_World::Destroy            },
    { "isDestroyed",        Wrap_World::IsDestroyed        }
};
// clang-format on

int Wrap_World::Register(lua_State* L)
{
    return luax::RegisterType(L, &World::type, functions);
}
