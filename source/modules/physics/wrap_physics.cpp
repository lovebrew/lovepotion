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

int Wrap_Physics::NewDistanceJoint(lua_State* L)
{
    Body* a = Wrap_Body::CheckBody(L, 1);
    Body* b = Wrap_Body::CheckBody(L, 2);

    float x1 = luaL_checknumber(L, 3);
    float y1 = luaL_checknumber(L, 4);
    float x2 = luaL_checknumber(L, 5);
    float y2 = luaL_checknumber(L, 6);

    bool collideConnected = Luax::OptBoolean(L, 7, false);
    DistanceJoint* joint  = nullptr;

    Luax::CatchException(
        L, [&]() { joint = instance()->NewDistanceJoint(a, b, x1, y1, x2, y2, collideConnected); });

    Luax::PushType(L, joint);
    joint->Release();

    return 1;
}

int Wrap_Physics::NewMouseJoint(lua_State* L)
{
    Body* body = Wrap_Body::CheckBody(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    MouseJoint* joint = nullptr;

    Luax::CatchException(L, [&]() { joint = instance()->NewMouseJoint(body, x, y); });

    Luax::PushType(L, joint);
    joint->Release();

    return 1;
}

int Wrap_Physics::NewRevoluteJoint(lua_State* L)
{
    Body* body1 = Wrap_Body::CheckBody(L, 1);
    Body* body2 = Wrap_Body::CheckBody(L, 2);

    float xA = luaL_checknumber(L, 3);
    float yA = luaL_checknumber(L, 4);

    float xB, yB;
    bool collideConnected;

    if (lua_gettop(L) >= 6)
    {
        xB = luaL_checknumber(L, 5);
        yB = luaL_checknumber(L, 6);

        collideConnected = Luax::OptBoolean(L, 7, false);
    }
    else
    {
        xB = xA;
        yB = yA;

        collideConnected = Luax::OptBoolean(L, 5, false);
    }

    RevoluteJoint* joint;
    Luax::CatchException(L, [&]() {
        if (lua_gettop(L) >= 8)
        {
            float referenceAngle = luaL_checknumber(L, 8);
            joint = instance()->NewRevoluteJoint(body1, body2, xA, yA, xB, yB, collideConnected,
                                                 referenceAngle);
        }
        else
            joint = instance()->NewRevoluteJoint(body1, body2, xA, yA, xB, yB, collideConnected);
    });

    Luax::PushType(L, joint);
    joint->Release();

    return 1;
}

int Wrap_Physics::NewPrismaticJoint(lua_State* L)
{
    Body* a = Wrap_Body::CheckBody(L, 1);
    Body* b = Wrap_Body::CheckBody(L, 2);

    float xA = luaL_checknumber(L, 3);
    float yA = luaL_checknumber(L, 4);

    float xB, yB, ax, ay;
    bool collideConnected;

    if (lua_gettop(L) >= 8)
    {
        xB = luaL_checknumber(L, 5);
        yB = luaL_checknumber(L, 6);
        ax = luaL_checknumber(L, 7);
        ay = luaL_checknumber(L, 8);

        collideConnected = Luax::OptBoolean(L, 9, false);
    }
    else
    {
        xB = xA;
        yB = yA;
        ax = luaL_checknumber(L, 5);
        ay = luaL_checknumber(L, 6);

        collideConnected = Luax::OptBoolean(L, 7, false);
    }

    PrismaticJoint* joint = nullptr;

    Luax::CatchException(L, [&]() {
        if (lua_gettop(L) >= 10)
        {
            float referenceAngle = luaL_checknumber(L, 10);
            joint = instance()->NewPrismaticJoint(a, b, xA, yA, xB, yB, ax, ay, collideConnected,
                                                  referenceAngle);
        }
        else
            joint = instance()->NewPrismaticJoint(a, b, xA, yA, xB, yB, ax, ay, collideConnected);
    });

    Luax::PushType(L, joint);
    joint->Release();

    return 1;
}

int Wrap_Physics::NewPulleyJoint(lua_State* L)
{
    Body* a = Wrap_Body::CheckBody(L, 1);
    Body* b = Wrap_Body::CheckBody(L, 2);

    float gx1   = luaL_checknumber(L, 3);
    float gy1   = luaL_checknumber(L, 4);
    float gx2   = luaL_checknumber(L, 5);
    float gy2   = luaL_checknumber(L, 6);
    float x1    = luaL_checknumber(L, 7);
    float y1    = luaL_checknumber(L, 8);
    float x2    = luaL_checknumber(L, 9);
    float y2    = luaL_checknumber(L, 10);
    float ratio = luaL_optnumber(L, 11, 1.0);

    /* PulleyJoints default to colliding connected bodies, see b2PulleyJoint.h */
    bool collideConnected = Luax::OptBoolean(L, 12, true);

    PulleyJoint* joint = nullptr;

    Luax::CatchException(L, [&]() {
        joint = instance()->NewPulleyJoint(a, b, b2Vec2(gx1, gy1), b2Vec2(gx2, gy2), b2Vec2(x1, y1),
                                           b2Vec2(x2, y2), ratio, collideConnected);
    });

    Luax::PushType(L, joint);
    joint->Release();

    return 1;
}

int Wrap_Physics::NewGearJoint(lua_State* L)
{
    Joint* a = Wrap_Joint::CheckJoint(L, 1);
    Joint* b = Wrap_Joint::CheckJoint(L, 2);

    float ratio           = luaL_optnumber(L, 3, 1.0);
    bool collideConnected = Luax::OptBoolean(L, 4, false);

    GearJoint* joint = nullptr;

    Luax::CatchException(
        L, [&]() { joint = instance()->NewGearJoint(a, b, ratio, collideConnected); });

    Luax::PushType(L, joint);
    joint->Release();

    return 1;
}

int Wrap_Physics::NewFrictionJoint(lua_State* L)
{
    Body* a = Wrap_Body::CheckBody(L, 1);
    Body* b = Wrap_Body::CheckBody(L, 2);

    float xA = luaL_checknumber(L, 3);
    float yA = luaL_checknumber(L, 4);

    float xB, yB;
    bool collideConnected;

    if (lua_gettop(L) >= 6)
    {
        xB = luaL_checknumber(L, 5);
        yB = luaL_checknumber(L, 6);

        collideConnected = Luax::OptBoolean(L, 7, false);
    }
    else
    {
        xB = xA;
        yB = yA;

        collideConnected = Luax::OptBoolean(L, 5, false);
    }

    FrictionJoint* joint = nullptr;

    Luax::CatchException(
        L, [&]() { joint = instance()->NewFrictionJoint(a, b, xA, yA, xB, yB, collideConnected); });

    Luax::PushType(L, joint);
    joint->Release();

    return 1;
}

int Wrap_Physics::NewWeldJoint(lua_State* L)
{
    Body* a = Wrap_Body::CheckBody(L, 1);
    Body* b = Wrap_Body::CheckBody(L, 2);

    float xA = luaL_checknumber(L, 3);
    float yA = luaL_checknumber(L, 4);

    float xB, yB;
    bool collideConnected;

    if (lua_gettop(L) >= 6)
    {
        xB = luaL_checknumber(L, 5);
        yB = luaL_checknumber(L, 6);

        collideConnected = Luax::OptBoolean(L, 7, false);
    }
    else
    {
        xB = xA;
        yB = yA;

        collideConnected = Luax::OptBoolean(L, 5, false);
    }

    WeldJoint* joint = nullptr;

    Luax::CatchException(L, [&]() {
        if (lua_gettop(L) >= 8)
        {
            float referenceAngle = luaL_checknumber(L, 8);
            joint =
                instance()->NewWeldJoint(a, b, xA, yA, xB, yB, collideConnected, referenceAngle);
        }
        else
            joint = instance()->NewWeldJoint(a, b, xA, yA, xB, yB, collideConnected);
    });

    Luax::PushType(L, joint);
    joint->Release();

    return 1;
}

int Wrap_Physics::NewWheelJoint(lua_State* L)
{
    Body* a = Wrap_Body::CheckBody(L, 1);
    Body* b = Wrap_Body::CheckBody(L, 2);

    float xA = luaL_checknumber(L, 3);
    float yA = luaL_checknumber(L, 4);

    float xB, yB, ax, ay;
    bool collideConnected;

    if (lua_gettop(L) >= 8)
    {
        xB = luaL_checknumber(L, 5);
        yB = luaL_checknumber(L, 6);
        ax = luaL_checknumber(L, 7);
        ay = luaL_checknumber(L, 8);

        collideConnected = Luax::OptBoolean(L, 9, false);
    }
    else
    {
        xB = xA;
        yB = yA;
        ax = luaL_checknumber(L, 5);
        ay = luaL_checknumber(L, 6);

        collideConnected = Luax::OptBoolean(L, 7, false);
    }

    WheelJoint* joint = nullptr;

    Luax::CatchException(L, [&]() {
        joint = instance()->NewWheelJoint(a, b, xA, yA, xB, yB, ax, ay, collideConnected);
    });

    Luax::PushType(L, joint);
    joint->Release();

    return 1;
}

int Wrap_Physics::NewRopeJoint(lua_State* L)
{
    Body* a = Wrap_Body::CheckBody(L, 1);
    Body* b = Wrap_Body::CheckBody(L, 2);

    float x1        = luaL_checknumber(L, 3);
    float y1        = luaL_checknumber(L, 4);
    float x2        = luaL_checknumber(L, 5);
    float y2        = luaL_checknumber(L, 6);
    float maxLength = luaL_checknumber(L, 7);

    bool collideConnected = Luax::OptBoolean(L, 8, false);

    RopeJoint* joint = nullptr;

    Luax::CatchException(L, [&]() {
        joint = instance()->NewRopeJoint(a, b, x1, y1, x2, y2, maxLength, collideConnected);
    });

    Luax::PushType(L, joint);
    joint->Release();

    return 1;
}

int Wrap_Physics::NewMotorJoint(lua_State* L)
{
    Body* a = Wrap_Body::CheckBody(L, 1);
    Body* b = Wrap_Body::CheckBody(L, 2);

    MotorJoint* joint = nullptr;

    if (!lua_isnoneornil(L, 3))
    {
        float correctionFactor = luaL_checknumber(L, 3);
        bool collideConnected  = Luax::OptBoolean(L, 4, false);

        Luax::CatchException(L, [&]() {
            joint = instance()->NewMotorJoint(a, b, correctionFactor, collideConnected);
        });
    }
    else
        Luax::CatchException(L, [&]() { joint = instance()->NewMotorJoint(a, b); });

    Luax::PushType(L, joint);
    joint->Release();

    return 1;
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

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getDistance",       Wrap_Physics::GetDistance       },
    { "getMeter",          Wrap_Physics::GetMeter          },
    { "newBody",           Wrap_Physics::NewBody           },
    { "newChainShape",     Wrap_Physics::NewChainShape     },
    { "newCircleShape",    Wrap_Physics::NewCircleShape    },
    { "newDistanceJoint",  Wrap_Physics::NewDistanceJoint  },
    { "newEdgeShape",      Wrap_Physics::NewEdgeShape      },
    { "newFixture",        Wrap_Physics::NewFixture        },
    { "newFrictionJoint",  Wrap_Physics::NewFrictionJoint  },
    { "newGearJoint",      Wrap_Physics::NewGearJoint      },
    { "newMotorJoint",     Wrap_Physics::NewMotorJoint     },
    { "newMouseJoint",     Wrap_Physics::NewMouseJoint     },
    { "newPolygonShape",   Wrap_Physics::NewPolygonShape   },
    { "newPrismaticJoint", Wrap_Physics::NewPrismaticJoint },
    { "newPulleyJoint",    Wrap_Physics::NewPulleyJoint    },
    { "newRectangleShape", Wrap_Physics::NewRectangleShape },
    { "newRevoluteJoint",  Wrap_Physics::NewRevoluteJoint  },
    { "newRopeJoint",      Wrap_Physics::NewRopeJoint      },
    { "newWeldJoint",      Wrap_Physics::NewWeldJoint      },
    { "newWheelJoint",     Wrap_Physics::NewWheelJoint     },
    { "newWorld",          Wrap_Physics::NewWorld          },
    { "setMeter",          Wrap_Physics::SetMeter          },
    { 0,                   0                               }
};

static constexpr lua_CFunction types[] =
{
    Wrap_Body::Register,
    Wrap_Contact::Register,
    Wrap_Fixture::Register,
    Wrap_Joint::Register,
    Wrap_Shape::Register,
    Wrap_ChainShape::Register,
    Wrap_CircleShape::Register,
    Wrap_DistanceJoint::Register,
    Wrap_EdgeShape::Register,
    Wrap_FrictionJoint::Register,
    Wrap_GearJoint::Register,
    Wrap_MotorJoint::Register,
    Wrap_MouseJoint::Register,
    Wrap_PolygonShape::Register,
    Wrap_PrismaticJoint::Register,
    Wrap_PulleyJoint::Register,
    Wrap_RopeJoint::Register,
    Wrap_WeldJoint::Register,
    Wrap_WheelJoint::Register,
    Wrap_World::Register,
    nullptr
};
// clang-format on

int Wrap_Physics::Register(lua_State* L)
{
    Physics* instance = instance();

    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new Physics(); });
    else
        instance->Retain();

    WrappedModule module;
    module.instance  = instance;
    module.name      = "physics";
    module.type      = &Module::type;
    module.functions = functions;
    module.types     = types;

    return Luax::RegisterModule(L, module);
}
