#include "modules/physics/wrap_physics.h"

using namespace love;

#define instance() (Module::GetInstance<Physics>(Module::M_PHYSICS))

int Wrap_Physics::NewWorld(lua_State* L)
{
    float gx = luaL_optnumber(L, 1, 0);
    float gy = luaL_optnumber(L, 2, 0);

    bool sleep = Luax::OptBoolean(L, 3, true);

    World* world = nullptr;

    Luax::CatchException(L, [&]() { world = instance()->NewWorld(gx, gy, sleep); });

    Luax::PushType(L, world);
    world->Release();

    return 1;
}

int Wrap_Physics::NewBody(lua_State* L)
{
    World* world = Wrap_World::CheckWorld(L, 1);

    float x = luaL_optnumber(L, 2, 0);
    float y = luaL_optnumber(L, 3, 0);

    Body::Type type     = Body::BODY_STATIC;
    const char* typeStr = lua_isnoneornil(L, 4) ? nullptr : luaL_checkstring(L, 4);

    if (typeStr && !Body::GetConstant(typeStr, type))
        return Luax::EnumError(L, "Body Type", Body::GetConstants(type), typeStr);

    Body* body = nullptr;

    Luax::CatchException(L, [&]() { body = instance()->NewBody(world, x, y, type); });

    Luax::PushType(L, body);
    body->Release();

    return 1;
}

int Wrap_Physics::NewFixture(lua_State* L)
{
    Body* body   = Wrap_Body::CheckBody(L, 1);
    Shape* shape = Wrap_Shape::CheckShape(L, 2);

    float density = luaL_optnumber(L, 3, 1.0f);

    Fixture* fixture = nullptr;

    Luax::CatchException(L, [&]() { fixture = instance()->NewFixture(body, shape, density); });

    Luax::PushType(L, fixture);
    fixture->Release();

    return 1;
}

int Wrap_Physics::NewCircleShape(lua_State* L)
{
    int argc = lua_gettop(L);

    if (argc == 1)
    {
        float radius = luaL_checknumber(L, 1);

        CircleShape* shape = nullptr;

        Luax::CatchException(L, [&]() { shape = instance()->NewCircleShape(radius); });

        Luax::PushType(L, shape);
        shape->Release();

        return 1;
    }
    else if (argc == 3)
    {
        float x = luaL_checknumber(L, 1);
        float y = luaL_checknumber(L, 2);

        float radius = luaL_checknumber(L, 3);

        CircleShape* shape = nullptr;

        Luax::CatchException(L, [&]() { shape = instance()->NewCircleShape(x, y, radius); });

        Luax::PushType(L, shape);
        shape->Release();

        return 1;
    }
    else
        return luaL_error(L, "Incorrect number of parameters.");
}

int Wrap_Physics::NewRectangleShape(lua_State* L)
{
    int argc = lua_gettop(L);

    if (argc == 2)
    {
        float width  = luaL_checknumber(L, 1);
        float height = luaL_checknumber(L, 2);

        PolygonShape* shape = nullptr;
        Luax::CatchException(L, [&]() { shape = instance()->NewRectangleShape(width, height); });

        Luax::PushType(L, shape);
        shape->Release();

        return 1;
    }
    else if (argc == 4 || argc == 5)
    {
        float x = luaL_checknumber(L, 1);
        float y = luaL_checknumber(L, 2);

        float width  = luaL_checknumber(L, 3);
        float height = luaL_checknumber(L, 4);

        float angle = luaL_optnumber(L, 5, 0);

        PolygonShape* shape = nullptr;
        Luax::CatchException(
            L, [&]() { shape = instance()->NewRectangleShape(x, y, width, height, angle); });

        Luax::PushType(L, shape);
        shape->Release();

        return 1;
    }
    else
        return luaL_error(L, "Incorrect number of parameters.");
}

int Wrap_Physics::NewEdgeShape(lua_State* L)
{
    float x1 = luaL_checknumber(L, 1);
    float y1 = luaL_checknumber(L, 1);
    float x2 = luaL_checknumber(L, 1);
    float y2 = luaL_checknumber(L, 1);

    bool oneSided = Luax::OptBoolean(L, 5, false);

    EdgeShape* shape = nullptr;
    Luax::CatchException(L, [&]() { shape = instance()->NewEdgeShape(x1, y1, x2, y2, oneSided); });

    Luax::PushType(L, shape);
    shape->Release();

    return 1;
}

int Wrap_Physics::NewPolygonShape(lua_State* L)
{
    int ret = 0;
    Luax::CatchException(L, [&]() { ret = instance()->NewPolygonShape(L); });

    return ret;
}

int Wrap_Physics::NewChainShape(lua_State* L)
{
    int ret = 0;
    Luax::CatchException(L, [&]() { ret = instance()->NewChainShape(L); });

    return ret;
}

int Wrap_Physics::GetDistance(lua_State* L)
{
    return instance()->GetDistance(L);
}

int Wrap_Physics::SetMeter(lua_State* L)
{
    float meter = luaL_checknumber(L, 1);
    Luax::CatchException(L, [&]() { Physics::SetMeter(meter); });

    return 0;
}

int Wrap_Physics::GetMeter(lua_State* L)
{
    lua_pushinteger(L, Physics::GetMeter());

    return 1;
}

int Wrap_Physics::Register(lua_State* L)
{
    luaL_Reg funcs[] = { { "newWorld", NewWorld },
                         { "newBody", NewBody },
                         { "newFixture", NewFixture },
                         { "newCircleShape", NewCircleShape },
                         { "newRectangleShape", NewRectangleShape },
                         { "newPolygonShape", NewPolygonShape },
                         { "newEdgeShape", NewEdgeShape },
                         { "newChainShape", NewChainShape },
                         { "getMeter", GetMeter },
                         { "setMeter", SetMeter },
                         { 0, 0 } };

    lua_CFunction types[] = { Wrap_Contact::Register,
                              Wrap_Fixture::Register,
                              Wrap_Shape::Register,
                              Wrap_Body::Register,
                              Wrap_Joint::Register,
                              Wrap_World::Register,
                              Wrap_EdgeShape::Register,
                              Wrap_ChainShape::Register,
                              Wrap_CircleShape::Register,
                              Wrap_PolygonShape::Register,
                              0 };

    Physics* instance = instance();

    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new Physics(); });
    else
        instance->Retain();

    WrappedModule module;
    module.instance  = instance;
    module.name      = "physics";
    module.type      = &Module::type;
    module.functions = funcs;
    module.types     = types;

    return Luax::RegisterModule(L, module);
}
