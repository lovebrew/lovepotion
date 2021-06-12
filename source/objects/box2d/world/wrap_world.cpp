#include "world/wrap_world.h"

using namespace love;

World* Wrap_World::CheckWorld(lua_State* L, int index)
{
    World* world = Luax::CheckType<World>(L, index);

    if (!world->IsValid())
        luaL_error(L, "Attempt to use destroyed world!");

    return world;
}

int Wrap_World::Update(lua_State* L)
{
    World* self = Wrap_World::CheckWorld(L, 1);
    float dt    = luaL_checknumber(L, 2);

    self->SetLuaCallbacks(L);

    if (lua_isnoneornil(L, 3))
        Luax::CatchException(L, [&]() { self->Update(dt); });
    else
    {
        int velocityIterations = luaL_checkinteger(L, 3);
        int positionIterations = luaL_checkinteger(L, 4);

        Luax::CatchException(L,
                             [&]() { self->Update(dt, velocityIterations, positionIterations); });
    }

    return 0;
}

int Wrap_World::SetCallbacks(lua_State* L)
{
    World* self = Wrap_World::CheckWorld(L, 1);
    lua_remove(L, 1);

    return self->SetCallbacks(L);
}

int Wrap_World::GetCallbacks(lua_State* L)
{
    World* self = Wrap_World::CheckWorld(L, 1);
    lua_remove(L, 1);

    return self->GetCallbacks(L);
}

int Wrap_World::SetContactFilter(lua_State* L)
{
    World* self = Wrap_World::CheckWorld(L, 1);
    lua_remove(L, 1);

    return self->SetContactFilter(L);
}

int Wrap_World::GetContactFilter(lua_State* L)
{
    World* self = Wrap_World::CheckWorld(L, 1);
    lua_remove(L, 1);

    return self->GetContactFilter(L);
}

int Wrap_World::SetGravity(lua_State* L)
{
    World* self = Wrap_World::CheckWorld(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    self->SetGravity(x, y);

    return 0;
}

int Wrap_World::GetGravity(lua_State* L)
{
    World* self = Wrap_World::CheckWorld(L, 1);
    lua_remove(L, 1);

    return self->GetGravity(L);
}

int Wrap_World::TranslateOrigin(lua_State* L)
{
    World* self = Wrap_World::CheckWorld(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    Luax::CatchException(L, [&]() { self->TranslateOrigin(x, y); });

    return 0;
}

int Wrap_World::SetSleepingAllowed(lua_State* L)
{
    World* self  = Wrap_World::CheckWorld(L, 1);
    bool allowed = Luax::CheckBoolean(L, 2);

    self->SetSleepingAllowed(allowed);

    return 0;
}

int Wrap_World::IsSleepingAllowed(lua_State* L)
{
    World* self = Wrap_World::CheckWorld(L, 1);

    Luax::PushBoolean(L, self->IsSleepingAllowed());

    return 1;
}

int Wrap_World::IsLocked(lua_State* L)
{
    World* self = Wrap_World::CheckWorld(L, 1);

    Luax::PushBoolean(L, self->IsLocked());

    return 1;
}

int Wrap_World::GetBodyCount(lua_State* L)
{
    World* self = Wrap_World::CheckWorld(L, 1);

    lua_pushinteger(L, self->GetBodyCount());

    return 1;
}

int Wrap_World::GetJointCount(lua_State* L)
{
    World* self = Wrap_World::CheckWorld(L, 1);

    lua_pushinteger(L, self->GetJointCount());

    return 1;
}

int Wrap_World::GetContactCount(lua_State* L)
{
    World* self = Wrap_World::CheckWorld(L, 1);

    lua_pushinteger(L, self->GetContactCount());

    return 1;
}

int Wrap_World::GetBodies(lua_State* L)
{
    World* self = Wrap_World::CheckWorld(L, 1);
    lua_remove(L, 1);

    int ret = 0;
    Luax::CatchException(L, [&]() { ret = self->GetBodies(L); });

    return ret;
}

int Wrap_World::GetJoints(lua_State* L)
{
    World* self = Wrap_World::CheckWorld(L, 1);
    lua_remove(L, 1);

    int ret = 0;
    Luax::CatchException(L, [&]() { ret = self->GetJoints(L); });

    return ret;
}

int Wrap_World::GetContacts(lua_State* L)
{
    World* self = Wrap_World::CheckWorld(L, 1);
    lua_remove(L, 1);

    int ret = 0;
    Luax::CatchException(L, [&]() { ret = self->GetContacts(L); });

    return ret;
}

int Wrap_World::QueryBoundingBox(lua_State* L)
{
    World* self = Wrap_World::CheckWorld(L, 1);
    lua_remove(L, 1);

    return self->QueryBoundingBox(L);
}

int Wrap_World::RayCast(lua_State* L)
{
    World* self = Wrap_World::CheckWorld(L, 1);
    lua_remove(L, 1);

    int ret = 0;
    Luax::CatchException(L, [&]() { ret = self->RayCast(L); });

    return ret;
}

int Wrap_World::Destroy(lua_State* L)
{
    World* self = Wrap_World::CheckWorld(L, 1);

    Luax::CatchException(L, [&]() { self->Destroy(); });

    return 0;
}

int Wrap_World::IsDestroyed(lua_State* L)
{
    World* self = Luax::CheckType<World>(L, 1);

    Luax::PushBoolean(L, !self->IsValid());

    return 1;
}

int Wrap_World::Register(lua_State* L)
{
    luaL_Reg funcs[] = { { "update", Update },
                         { "setCallbacks", SetCallbacks },
                         { "getCallbacks", GetCallbacks },
                         { "setContactFilter", SetContactFilter },
                         { "getContactFilter", GetContactFilter },
                         { "setGravity", SetGravity },
                         { "getGravity", GetGravity },
                         { "translateOrigin", TranslateOrigin },
                         { "setSleepingAllowed", SetSleepingAllowed },
                         { "isSleepingAllowed", IsSleepingAllowed },
                         { "isLocked", IsLocked },
                         { "getBodyCount", GetBodyCount },
                         { "getJointCount", GetJointCount },
                         { "getContactCount", GetContactCount },
                         { "getBodies", GetBodies },
                         { "getJoints", GetJoints },
                         { "getContacts", GetContacts },
                         { "queryBoundingBox", QueryBoundingBox },
                         { "rayCast", RayCast },
                         { "destroy", Destroy },
                         { "isDestroyed", IsDestroyed },

                         { 0, 0 } };

    return Luax::RegisterType(L, &World::type, funcs, nullptr);
}
