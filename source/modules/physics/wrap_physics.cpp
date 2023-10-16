#include <modules/physics/wrap_physics.hpp>

#include <objects/body/wrap_body.hpp>
#include <objects/contact/wrap_contact.hpp>

#include <objects/joint/types/distancejoint/wrap_distancejoint.hpp>
#include <objects/joint/types/frictionjoint/wrap_frictionjoint.hpp>
#include <objects/joint/types/gearjoint/wrap_gearjoint.hpp>
#include <objects/joint/types/motorjoint/wrap_motorjoint.hpp>
#include <objects/joint/types/mousejoint/wrap_mousejoint.hpp>
#include <objects/joint/types/prismaticjoint/wrap_prismaticjoint.hpp>
#include <objects/joint/types/pulleyjoint/wrap_pulleyjoint.hpp>
#include <objects/joint/types/revolutejoint/wrap_revolutejoint.hpp>
#include <objects/joint/types/ropejoint/wrap_ropejoint.hpp>
#include <objects/joint/types/weldjoint/wrap_weldjoint.hpp>
#include <objects/joint/types/wheeljoint/wrap_wheeljoint.hpp>
#include <objects/joint/wrap_joint.hpp>

#include <objects/shape/types/chainshape/wrap_chainshape.hpp>
#include <objects/shape/types/circleshape/wrap_circleshape.hpp>
#include <objects/shape/types/edgeshape/wrap_edgeshape.hpp>
#include <objects/shape/types/polygonshape/wrap_polygonshape.hpp>
#include <objects/shape/wrap_shape.hpp>

#include <objects/world/wrap_world.hpp>

using namespace love;

#define instance() (Module::GetInstance<Physics>(Module::M_PHYSICS))

int Wrap_Physics::NewWorld(lua_State* L)
{
    float gravityX = luaL_optnumber(L, 1, 0.0f);
    float gravityY = luaL_optnumber(L, 2, 0.0f);
    bool sleep     = luax::OptBoolean(L, 3, true);

    World* world = nullptr;

    luax::CatchException(L, [&]() { world = instance()->NewWorld(gravityX, gravityY, sleep); });

    luax::PushType(L, world);
    world->Release();

    return 1;
}

// #region Body

int Wrap_Physics::NewBody(lua_State* L)
{
    auto* world = Wrap_World::CheckWorld(L, 1);
    float x     = luaL_optnumber(L, 2, 0.0f);
    float y     = luaL_optnumber(L, 3, 0.0f);

    const char* typeString = lua_isnoneornil(L, 4) ? nullptr : luaL_checkstring(L, 4);

    std::optional<Body::Type> bodyType(Body::BODY_STATIC);
    if (typeString && (!(bodyType = Body::bodyTypes.Find(typeString))))
        return luax::EnumError(L, "Body type", Body::bodyTypes, typeString);

    Body* body = nullptr;
    luax::CatchException(L, [&]() { body = instance()->NewBody(world, x, y, *bodyType); });

    luax::PushType(L, body);
    body->Release();

    return 1;
}

int Wrap_Physics::NewCircleBody(lua_State* L)
{
    auto* world            = Wrap_World::CheckWorld(L, 1);
    const char* typeString = luaL_checkstring(L, 2);

    std::optional<Body::Type> bodyType(Body::BODY_STATIC);
    if (!(bodyType = Body::bodyTypes.Find(typeString)))
        return luax::EnumError(L, "Body type", Body::bodyTypes, typeString);

    float x      = luaL_checknumber(L, 3);
    float y      = luaL_checknumber(L, 4);
    float radius = luaL_checknumber(L, 5);

    Body* body = nullptr;
    luax::CatchException(
        L, [&]() { body = instance()->NewCircleBody(world, *bodyType, x, y, radius); });

    luax::PushType(L, body);
    body->Release();

    return 1;
}

int Wrap_Physics::NewRectangleBody(lua_State* L)
{
    auto* world            = Wrap_World::CheckWorld(L, 1);
    const char* typeString = luaL_checkstring(L, 2);

    std::optional<Body::Type> bodyType(Body::BODY_STATIC);
    if (!(bodyType = Body::bodyTypes.Find(typeString)))
        return luax::EnumError(L, "Body type", Body::bodyTypes, typeString);

    float x      = luaL_checknumber(L, 3);
    float y      = luaL_checknumber(L, 4);
    float width  = luaL_checknumber(L, 5);
    float height = luaL_checknumber(L, 6);
    float angle  = luaL_optnumber(L, 7, 0.0f);

    Body* body = nullptr;
    luax::CatchException(L, [&]() {
        body = instance()->NewRectangleBody(world, *bodyType, x, y, width, height, angle);
    });

    luax::PushType(L, body);
    body->Release();

    return 1;
}

int Wrap_Physics::NewPolygonBody(lua_State* L)
{
    auto* world            = Wrap_World::CheckWorld(L, 1);
    const char* typeString = luaL_checkstring(L, 2);

    std::optional<Body::Type> bodyType(Body::BODY_STATIC);
    if (!(bodyType = Body::bodyTypes.Find(typeString)))
        return luax::EnumError(L, "Body type", Body::bodyTypes, typeString);

    auto count         = lua_gettop(L);
    const auto isTable = lua_istable(L, 3);

    if (isTable)
        count = (int)luax::ObjectLength(L, 3);

    if (count % 2 != 0)
        return luaL_error(L, "Number of vertex components must be a multiple of two.");

    int vertexCount = count / 2;
    std::vector<Vector2> coords {};

    if (isTable)
    {
        for (int index = 0; index < vertexCount; index++)
        {
            lua_rawgeti(L, 3, 1 + index * 2);
            lua_rawgeti(L, 3, 2 + index * 2);

            float x = luaL_checknumber(L, -2);
            float y = luaL_checknumber(L, -1);

            coords.emplace_back(x, y);
            lua_pop(L, 2);
        }
    }
    else
    {
        for (int index = 0; index < vertexCount; index++)
        {
            float x = luaL_checknumber(L, 3 + index * 2);
            float y = luaL_checknumber(L, 4 + index * 2);

            coords.emplace_back(x, y);
        }
    }

    Body* body = nullptr;

    luax::CatchException(L, [&]() { body = instance()->NewPolygonBody(world, *bodyType, coords); });

    luax::PushType(L, body);
    body->Release();

    return 1;
}

int Wrap_Physics::NewEdgeBody(lua_State* L)
{
    auto* world            = Wrap_World::CheckWorld(L, 1);
    const char* typeString = luaL_checkstring(L, 2);

    std::optional<Body::Type> bodyType(Body::BODY_STATIC);
    if (!(bodyType = Body::bodyTypes.Find(typeString)))
        return luax::EnumError(L, "Body type", Body::bodyTypes, typeString);

    float x1 = luaL_checknumber(L, 3);
    float y1 = luaL_checknumber(L, 4);
    float x2 = luaL_checknumber(L, 5);
    float y2 = luaL_checknumber(L, 6);

    bool oneSided = luax::OptBoolean(L, 7, false);

    Body* body = nullptr;

    luax::CatchException(
        L, [&]() { body = instance()->NewEdgeBody(world, *bodyType, x1, y1, x2, y2, oneSided); });

    luax::PushType(L, body);
    body->Release();

    return 1;
}

int Wrap_Physics::NewChainBody(lua_State* L)
{
    auto* world            = Wrap_World::CheckWorld(L, 1);
    const char* typeString = luaL_checkstring(L, 2);

    std::optional<Body::Type> bodyType(Body::BODY_STATIC);
    if (!(bodyType = Body::bodyTypes.Find(typeString)))
        return luax::EnumError(L, "Body type", Body::bodyTypes, typeString);

    const auto loop = luax::CheckBoolean(L, 3);
    auto count      = lua_gettop(L) - 3;

    const auto isTable = lua_istable(L, 4);
    if (isTable)
        count = (int)luax::ObjectLength(L, 4);

    if (count == 0 || ((count % 2) != 0))
        return luaL_error(L, "Number of vertex components must be a multiple of two.");

    const auto vertexCount = count / 2;
    std::vector<Vector2> coords {};

    if (isTable)
    {
        for (int index = 0; index < vertexCount; index++)
        {
            lua_rawgeti(L, 4, 1 + index * 2);
            lua_rawgeti(L, 4, 2 + index * 2);

            float x = lua_tonumber(L, -2);
            float y = lua_tonumber(L, -1);

            coords.emplace_back(x, y);
            lua_pop(L, 2);
        }
    }
    else
    {
        for (int index = 0; index < vertexCount; index++)
        {
            float x = luaL_checknumber(L, 4 + index * 2);
            float y = luaL_checknumber(L, 5 + index * 2);

            coords.emplace_back(x, y);
        }
    }

    Body* body = nullptr;
    luax::CatchException(
        L, [&]() { body = instance()->NewChainBody(world, *bodyType, loop, coords); });

    luax::PushType(L, body);
    body->Release();

    return 1;
}

// #endregion Body

// #region Shape

static Body* optBodyForShape(lua_State* L, int index)
{
    if (lua_isnoneornil(L, index) || luax::IsType(L, index, Object::type))
        return Wrap_Body::CheckBody(L, index);

    return nullptr;
}

int Wrap_Physics::NewCircleShape(lua_State* L)
{
    auto* body    = optBodyForShape(L, 1);
    int bodyIndex = body ? 1 : 0;

    const auto top = lua_gettop(L) - bodyIndex;

    if (top == 1)
    {
        float radius       = luaL_checknumber(L, bodyIndex + 1);
        CircleShape* shape = nullptr;

        luax::CatchException(
            L, [&]() { shape = instance()->NewCircleShape(body, 0.0f, 0.0f, radius); });

        luax::PushType(L, shape);
        shape->Release();

        return 1;
    }
    else if (top == 3)
    {
        float x      = luaL_checknumber(L, bodyIndex + 1);
        float y      = luaL_checknumber(L, bodyIndex + 2);
        float radius = luaL_checknumber(L, bodyIndex + 3);

        CircleShape* shape = nullptr;

        luax::CatchException(L, [&]() { shape = instance()->NewCircleShape(body, x, y, radius); });

        luax::PushType(L, shape);
        shape->Release();

        return 1;
    }
    else
        return luaL_error(L, "Invalid number of parameters.");
}

int Wrap_Physics::NewRectangleShape(lua_State* L)
{
    auto* body    = optBodyForShape(L, 1);
    int bodyIndex = body ? 1 : 0;

    const auto top = lua_gettop(L) - bodyIndex;

    if (top == 2)
    {
        float width         = luaL_checknumber(L, bodyIndex + 1);
        float height        = luaL_checknumber(L, bodyIndex + 2);
        PolygonShape* shape = nullptr;

        luax::CatchException(L, [&]() {
            shape = instance()->NewRectangleShape(body, 0.0f, 0.0f, width, height, 0.0f);
        });

        luax::PushType(L, shape);
        shape->Release();

        return 1;
    }
    else if (top == 4 || top == 5)
    {
        float x      = luaL_checknumber(L, bodyIndex + 1);
        float y      = luaL_checknumber(L, bodyIndex + 2);
        float width  = luaL_checknumber(L, bodyIndex + 3);
        float height = luaL_checknumber(L, bodyIndex + 4);
        float angle  = luaL_optnumber(L, bodyIndex + 5, 0.0f);

        PolygonShape* shape = nullptr;
        luax::CatchException(
            L, [&]() { shape = instance()->NewRectangleShape(body, x, y, width, height, angle); });

        luax::PushType(L, shape);
        shape->Release();

        return 1;
    }
    else
        return luaL_error(L, "Invalid number of parameters.");
}

int Wrap_Physics::NewEdgeShape(lua_State* L)
{
    auto* body    = optBodyForShape(L, 1);
    int bodyIndex = body ? 1 : 0;

    float x1      = luaL_checknumber(L, bodyIndex + 1);
    float y1      = luaL_checknumber(L, bodyIndex + 2);
    float x2      = luaL_checknumber(L, bodyIndex + 3);
    float y2      = luaL_checknumber(L, bodyIndex + 4);
    bool oneSided = luax::OptBoolean(L, bodyIndex + 5, false);

    EdgeShape* shape = nullptr;
    luax::CatchException(
        L, [&]() { shape = instance()->NewEdgeShape(body, x1, y1, x2, y2, oneSided); });

    luax::PushType(L, shape);
    shape->Release();

    return 1;
}

int Wrap_Physics::NewPolygonShape(lua_State* L)
{
    auto* body    = optBodyForShape(L, 1);
    int bodyIndex = body ? 1 : 0;

    auto top           = lua_gettop(L) - bodyIndex;
    const auto isTable = lua_istable(L, bodyIndex + 1);

    if (isTable)
        top = (int)luax::ObjectLength(L, bodyIndex + 1);

    if ((top % 2) != 0)
        return luaL_error(L, "Number of vertex components must be a multiple of two.");

    const auto vertexCount = top / 2;
    std::vector<Vector2> coords {};

    if (isTable)
    {
        for (int index = 0; index < vertexCount; index++)
        {
            lua_rawgeti(L, bodyIndex + 1, 1 + index * 2);
            lua_rawgeti(L, bodyIndex + 1, 2 + index * 2);

            float x = luaL_checknumber(L, -2);
            float y = luaL_checknumber(L, -1);

            coords.emplace_back(x, y);
            lua_pop(L, 2);
        }
    }
    else
    {
        for (int index = 0; index < vertexCount; index++)
        {
            float x = luaL_checknumber(L, bodyIndex + 1 + index * 2);
            float y = luaL_checknumber(L, bodyIndex + 2 + index * 2);

            coords.emplace_back(x, y);
        }
    }

    PolygonShape* shape = nullptr;
    luax::CatchException(L, [&]() { shape = instance()->NewPolygonShape(body, coords); });

    luax::PushType(L, shape);
    shape->Release();

    return 1;
}

int Wrap_Physics::NewChainShape(lua_State* L)
{
    auto* body    = optBodyForShape(L, 1);
    int bodyIndex = body ? 1 : 0;

    auto top     = lua_gettop(L) - 1 - bodyIndex;
    bool isTable = lua_istable(L, bodyIndex + 2);

    if (isTable)
        top = (int)luax::ObjectLength(L, bodyIndex + 2);

    if (top == 0 || ((top % 2) != 0))
        return luaL_error(L, "Number of vertex components must be a multiple of two.");

    const auto vertexCount = top / 2;
    std::vector<Vector2> coords {};

    const auto loop = luax::CheckBoolean(L, bodyIndex + 1);

    if (isTable)
    {
        for (int index = 0; index < vertexCount; index++)
        {
            lua_rawgeti(L, bodyIndex + 2, 1 + index * 2);
            lua_rawgeti(L, bodyIndex + 2, 2 + index * 2);

            float x = luaL_checknumber(L, -2);
            float y = luaL_checknumber(L, -1);

            coords.emplace_back(x, y);
            lua_pop(L, 2);
        }
    }
    else
    {
        for (int index = 0; index < vertexCount; index++)
        {
            float x = luaL_checknumber(L, bodyIndex + 2 + index * 2);
            float y = luaL_checknumber(L, bodyIndex + 3 + index * 2);

            coords.emplace_back(x, y);
        }
    }

    ChainShape* shape = nullptr;
    luax::CatchException(L, [&]() { shape = instance()->NewChainShape(body, loop, coords); });

    luax::PushType(L, shape);
    shape->Release();

    return 1;
}

// #endregion Shape

// #region Joint

int Wrap_Physics::NewDistanceJoint(lua_State* L)
{
    auto* bodyA = Wrap_Body::CheckBody(L, 1);
    auto* bodyB = Wrap_Body::CheckBody(L, 2);

    float x1              = luaL_checknumber(L, 3);
    float y1              = luaL_checknumber(L, 4);
    float x2              = luaL_checknumber(L, 5);
    float y2              = luaL_checknumber(L, 6);
    bool collideConnected = luax::OptBoolean(L, 7, false);

    DistanceJoint* joint = nullptr;
    luax::CatchException(L, [&]() {
        joint = instance()->NewDistanceJoint(bodyA, bodyB, x1, y1, x2, y2, collideConnected);
    });

    luax::PushType(L, joint);
    joint->Release();

    return 1;
}

int Wrap_Physics::NewFrictionJoint(lua_State* L)
{
    auto* bodyA = Wrap_Body::CheckBody(L, 1);
    auto* bodyB = Wrap_Body::CheckBody(L, 2);

    float x1 = luaL_checknumber(L, 3);
    float y1 = luaL_checknumber(L, 4);

    float x2, y2 = 0.0f;
    bool collideConnected = false;

    if (lua_gettop(L) >= 6)
    {
        x2               = luaL_checknumber(L, 5);
        y2               = luaL_checknumber(L, 6);
        collideConnected = luax::OptBoolean(L, 7, false);
    }
    else
    {
        x2               = x1;
        y2               = y1;
        collideConnected = luax::OptBoolean(L, 5, false);
    }

    FrictionJoint* joint = nullptr;
    luax::CatchException(L, [&]() {
        joint = instance()->NewFrictionJoint(bodyA, bodyB, x1, y1, x2, y2, collideConnected);
    });

    luax::PushType(L, joint);
    joint->Release();

    return 1;
}

int Wrap_Physics::NewGearJoint(lua_State* L)
{
    auto* jointA          = Wrap_Joint::CheckJoint(L, 1);
    auto* jointB          = Wrap_Joint::CheckJoint(L, 2);
    float ratio           = luaL_checknumber(L, 3);
    bool collideConnected = luax::OptBoolean(L, 4, false);

    GearJoint* joint = nullptr;
    luax::CatchException(
        L, [&]() { joint = instance()->NewGearJoint(jointA, jointB, ratio, collideConnected); });

    luax::PushType(L, joint);
    joint->Release();

    return 1;
}

int Wrap_Physics::NewMotorJoint(lua_State* L)
{
    auto* bodyA = Wrap_Body::CheckBody(L, 1);
    auto* bodyB = Wrap_Body::CheckBody(L, 2);

    MotorJoint* joint = nullptr;
    if (!lua_isnoneornil(L, 3))
    {
        float correctionFactor = luaL_checknumber(L, 3);
        bool collideConnected  = luax::OptBoolean(L, 4, false);

        luax::CatchException(L, [&]() {
            joint = instance()->NewMotorJoint(bodyA, bodyB, correctionFactor, collideConnected);
        });
    }
    else
        luax::CatchException(L, [&]() { joint = instance()->NewMotorJoint(bodyA, bodyB); });

    luax::PushType(L, joint);
    joint->Release();

    return 1;
}

int Wrap_Physics::NewMouseJoint(lua_State* L)
{
    auto* body = Wrap_Body::CheckBody(L, 1);
    float x    = luaL_checknumber(L, 2);
    float y    = luaL_checknumber(L, 3);

    MouseJoint* joint = nullptr;
    luax::CatchException(L, [&]() { joint = instance()->NewMouseJoint(body, x, y); });

    luax::PushType(L, joint);
    joint->Release();

    return 1;
}

int Wrap_Physics::NewPrismaticJoint(lua_State* L)
{
    auto* bodyA = Wrap_Body::CheckBody(L, 1);
    auto* bodyB = Wrap_Body::CheckBody(L, 2);

    float xA = luaL_checknumber(L, 3);
    float yA = luaL_checknumber(L, 4);

    float xB, yB, aX, aY = 0.0f;
    bool collideConnected = false;

    if (lua_gettop(L) >= 8)
    {
        xB               = luaL_checknumber(L, 5);
        yB               = luaL_checknumber(L, 6);
        aX               = luaL_checknumber(L, 7);
        aY               = luaL_checknumber(L, 8);
        collideConnected = luax::OptBoolean(L, 9, false);
    }
    else
    {
        xB               = xA;
        yB               = yA;
        aX               = luaL_checknumber(L, 5);
        aY               = luaL_checknumber(L, 6);
        collideConnected = luax::OptBoolean(L, 7, false);
    }

    PrismaticJoint* joint = nullptr;
    luax::CatchException(L, [&]() {
        if (lua_gettop(L) >= 10)
        {
            float referenceAngle = luaL_checknumber(L, 10);
            joint = instance()->NewPrismaticJoint(bodyA, bodyB, xA, yA, xB, yB, aX, aY,
                                                  referenceAngle, collideConnected);
        }
        else
            joint = instance()->NewPrismaticJoint(bodyA, bodyB, xA, yA, xB, yB, aX, aY,
                                                  collideConnected);
    });

    luax::PushType(L, joint);
    joint->Release();

    return 1;
}

int Wrap_Physics::NewPulleyJoint(lua_State* L)
{
    auto* bodyA = Wrap_Body::CheckBody(L, 1);
    auto* bodyB = Wrap_Body::CheckBody(L, 2);

    float groundX1        = luaL_checknumber(L, 3);
    float groundY1        = luaL_checknumber(L, 4);
    float groundX2        = luaL_checknumber(L, 5);
    float groundY2        = luaL_checknumber(L, 6);
    float x1              = luaL_checknumber(L, 7);
    float y1              = luaL_checknumber(L, 8);
    float x2              = luaL_checknumber(L, 9);
    float y2              = luaL_checknumber(L, 10);
    float ratio           = luaL_checknumber(L, 11);
    bool collideConnected = luax::OptBoolean(L, 12, false);

    PulleyJoint* joint = nullptr;
    luax::CatchException(L, [&]() {
        const auto groundA = b2Vec2(groundX1, groundY1);
        const auto groundB = b2Vec2(groundX2, groundY2);

        const auto anchorA = b2Vec2(x1, y1);
        const auto anchorB = b2Vec2(x2, y2);

        joint = instance()->NewPulleyJoint(bodyA, bodyB, groundA, groundB, anchorA, anchorB, ratio,
                                           collideConnected);
    });

    luax::PushType(L, joint);
    joint->Release();

    return 1;
}

int Wrap_Physics::NewRevoluteJoint(lua_State* L)
{
    auto* bodyA = Wrap_Body::CheckBody(L, 1);
    auto* bodyB = Wrap_Body::CheckBody(L, 2);

    float xA = luaL_checknumber(L, 3);
    float yA = luaL_checknumber(L, 4);

    float xB, yB = 0.0f;
    bool collideConnected = false;

    if (lua_gettop(L) >= 6)
    {
        xB               = luaL_checknumber(L, 5);
        yB               = luaL_checknumber(L, 6);
        collideConnected = luax::OptBoolean(L, 7, false);
    }
    else
    {
        xB               = xA;
        yB               = yA;
        collideConnected = luax::OptBoolean(L, 5, false);
    }

    RevoluteJoint* joint = nullptr;
    luax::CatchException(L, [&]() {
        if (lua_gettop(L) >= 8)
        {
            float referenceAngle = luaL_checknumber(L, 8);
            joint = instance()->NewRevoluteJoint(bodyA, bodyB, xA, yA, xB, yB, referenceAngle,
                                                 collideConnected);
        }
        else
            joint = instance()->NewRevoluteJoint(bodyA, bodyB, xA, yA, xB, yB, collideConnected);
    });

    luax::PushType(L, joint);
    joint->Release();

    return 1;
}

int Wrap_Physics::NewRopeJoint(lua_State* L)
{
    auto* bodyA = Wrap_Body::CheckBody(L, 1);
    auto* bodyB = Wrap_Body::CheckBody(L, 2);

    float x1              = luaL_checknumber(L, 3);
    float y1              = luaL_checknumber(L, 4);
    float x2              = luaL_checknumber(L, 5);
    float y2              = luaL_checknumber(L, 6);
    float maxLength       = luaL_checknumber(L, 7);
    bool collideConnected = luax::OptBoolean(L, 8, false);

    RopeJoint* joint = nullptr;
    luax::CatchException(L, [&]() {
        joint = instance()->NewRopeJoint(bodyA, bodyB, x1, y1, x2, y2, maxLength, collideConnected);
    });

    luax::PushType(L, joint);
    joint->Release();

    return 1;
}

int Wrap_Physics::NewWeldJoint(lua_State* L)
{
    auto* bodyA = Wrap_Body::CheckBody(L, 1);
    auto* bodyB = Wrap_Body::CheckBody(L, 2);

    float xA = luaL_checknumber(L, 3);
    float yA = luaL_checknumber(L, 4);

    float xB, yB = 0.0f;
    bool collideConnected = false;

    if (lua_gettop(L) >= 6)
    {
        xB               = luaL_checknumber(L, 5);
        yB               = luaL_checknumber(L, 6);
        collideConnected = luax::OptBoolean(L, 7, false);
    }
    else
    {
        xB               = xA;
        yB               = yA;
        collideConnected = luax::OptBoolean(L, 5, false);
    }

    WeldJoint* joint = nullptr;
    luax::CatchException(L, [&]() {
        if (lua_gettop(L) >= 8)
        {
            float referenceAngle = luaL_checknumber(L, 8);
            joint = instance()->NewWeldJoint(bodyA, bodyB, xA, yA, xB, yB, collideConnected,
                                             referenceAngle);
        }
        else
            joint = instance()->NewWeldJoint(bodyA, bodyB, xA, yA, xB, yB, collideConnected);
    });

    luax::PushType(L, joint);
    joint->Release();

    return 1;
}

int Wrap_Physics::NewWheelJoint(lua_State* L)
{
    auto* bodyA = Wrap_Body::CheckBody(L, 1);
    auto* bodyB = Wrap_Body::CheckBody(L, 2);

    float xA = luaL_checknumber(L, 3);
    float yA = luaL_checknumber(L, 4);

    float xB, yB, aX, aY = 0.0f;
    bool collideConnected = false;

    if (lua_gettop(L) >= 8)
    {
        xB               = luaL_checknumber(L, 5);
        yB               = luaL_checknumber(L, 6);
        aX               = luaL_checknumber(L, 7);
        aY               = luaL_checknumber(L, 8);
        collideConnected = luax::OptBoolean(L, 9, false);
    }
    else
    {
        xB               = xA;
        yB               = yA;
        aX               = luaL_checknumber(L, 5);
        aY               = luaL_checknumber(L, 6);
        collideConnected = luax::OptBoolean(L, 7, false);
    }

    WheelJoint* joint = nullptr;
    luax::CatchException(L, [&]() {
        joint = instance()->NewWheelJoint(bodyA, bodyB, xA, yA, xB, yB, aX, aY, collideConnected);
    });

    luax::PushType(L, joint);
    joint->Release();

    return 1;
}

// #endregion Joint

int Wrap_Physics::GetDistance(lua_State* L)
{
    return instance()->GetDistance(L);
}

int Wrap_Physics::SetMeter(lua_State* L)
{
    float meter = luaL_checknumber(L, 1);
    instance()->SetMeter(meter);

    return 0;
}

int Wrap_Physics::GetMeter(lua_State* L)
{
    lua_pushnumber(L, instance()->GetMeter());

    return 1;
}

int Wrap_Physics::ComputeLinearStiffness(lua_State* L)
{
    float frequency    = luaL_checknumber(L, 1);
    float dampingRatio = luaL_checknumber(L, 2);

    Body* first    = Wrap_Body::CheckBody(L, 3);
    b2Body* second = nullptr;

    if (lua_isnoneornil(L, 4))
        second = first->GetWorld()->GetGroundBody();
    else
        second = Wrap_Body::CheckBody(L, 4)->body;

    float siffness, damping = 0.0f;
    Physics::ComputeLinearStiffness(siffness, damping, frequency, dampingRatio, first->body,
                                    second);

    lua_pushnumber(L, siffness);
    lua_pushnumber(L, damping);

    return 2;
}

int Wrap_Physics::ComputeLinearFrequency(lua_State* L)
{
    float stiffness = luaL_checknumber(L, 1);
    float damping   = luaL_checknumber(L, 2);

    Body* first    = Wrap_Body::CheckBody(L, 3);
    b2Body* second = nullptr;

    if (lua_isnoneornil(L, 4))
        second = first->GetWorld()->GetGroundBody();
    else
        second = Wrap_Body::CheckBody(L, 4)->body;

    float frequency, dampingRatio = 0.0f;
    Physics::ComputeLinearFrequency(frequency, dampingRatio, stiffness, damping, first->body,
                                    second);

    lua_pushnumber(L, frequency);
    lua_pushnumber(L, dampingRatio);

    return 2;
}

int Wrap_Physics::ComputeAngularStiffness(lua_State* L)
{
    float frequency    = luaL_checknumber(L, 1);
    float dampingRatio = luaL_checknumber(L, 2);

    Body* first    = Wrap_Body::CheckBody(L, 3);
    b2Body* second = nullptr;

    if (lua_isnoneornil(L, 4))
        second = first->GetWorld()->GetGroundBody();
    else
        second = Wrap_Body::CheckBody(L, 4)->body;

    float siffness, damping = 0.0f;
    Physics::ComputeAngularStiffness(siffness, damping, frequency, dampingRatio, first->body,
                                     second);

    lua_pushnumber(L, siffness);
    lua_pushnumber(L, damping);

    return 2;
}

int Wrap_Physics::ComputeAngularFrequency(lua_State* L)
{
    float stiffness = luaL_checknumber(L, 1);
    float damping   = luaL_checknumber(L, 2);

    Body* first    = Wrap_Body::CheckBody(L, 3);
    b2Body* second = nullptr;

    if (lua_isnoneornil(L, 4))
        second = first->GetWorld()->GetGroundBody();
    else
        second = Wrap_Body::CheckBody(L, 4)->body;

    float frequency, dampingRatio = 0.0f;
    Physics::ComputeAngularFrequency(frequency, dampingRatio, stiffness, damping, first->body,
                                     second);

    lua_pushnumber(L, frequency);
    lua_pushnumber(L, dampingRatio);

    return 2;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "newWorld",                Wrap_Physics::NewWorld                },
    { "newBody",                 Wrap_Physics::NewBody                 },
    { "newCircleBody",           Wrap_Physics::NewCircleBody           },
    { "newRectangleBody",        Wrap_Physics::NewRectangleBody        },
    { "newPolygonBody",          Wrap_Physics::NewPolygonBody          },
    { "newEdgeBody",             Wrap_Physics::NewEdgeBody             },
    { "newChainBody",            Wrap_Physics::NewChainBody            },
    { "newCircleShape",          Wrap_Physics::NewCircleShape          },
    { "newRectangleShape",       Wrap_Physics::NewRectangleShape       },
    { "newEdgeShape",            Wrap_Physics::NewEdgeShape            },
    { "newPolygonShape",         Wrap_Physics::NewPolygonShape         },
    { "newChainShape",           Wrap_Physics::NewChainShape           },
    { "newDistanceJoint",        Wrap_Physics::NewDistanceJoint        },
    { "newFrictionJoint",        Wrap_Physics::NewFrictionJoint        },
    { "newGearJoint",            Wrap_Physics::NewGearJoint            },
    { "newMotorJoint",           Wrap_Physics::NewMotorJoint           },
    { "newMouseJoint",           Wrap_Physics::NewMouseJoint           },
    { "newPrismaticJoint",       Wrap_Physics::NewPrismaticJoint       },
    { "newPulleyJoint",          Wrap_Physics::NewPulleyJoint          },
    { "newRevoluteJoint",        Wrap_Physics::NewRevoluteJoint        },
    { "newRopeJoint",            Wrap_Physics::NewRopeJoint            },
    { "newWeldJoint",            Wrap_Physics::NewWeldJoint            },
    { "newWheelJoint",           Wrap_Physics::NewWheelJoint           },
    { "getDistance",             Wrap_Physics::GetDistance             },
    { "getMeter",                Wrap_Physics::GetMeter                },
    { "setMeter",                Wrap_Physics::SetMeter                },
    { "computeLinearStiffness",  Wrap_Physics::ComputeLinearStiffness  },
    { "computeLinearFrequency",  Wrap_Physics::ComputeLinearFrequency  },
    { "computeAngularStiffness", Wrap_Physics::ComputeAngularStiffness },
    { "computeAngularFrequency", Wrap_Physics::ComputeAngularFrequency }
};

static constexpr lua_CFunction types[] =
{
    Wrap_World::Register,
    Wrap_Contact::Register,
    Wrap_Body::Register,
    Wrap_Shape::Register,
    Wrap_CircleShape::Register,
    Wrap_PolygonShape::Register,
    Wrap_EdgeShape::Register,
    Wrap_ChainShape::Register,
    Wrap_Joint::Register,
    Wrap_DistanceJoint::Register,
    Wrap_FrictionJoint::Register,
    Wrap_GearJoint::Register,
    Wrap_MotorJoint::Register,
    Wrap_MouseJoint::Register,
    Wrap_PrismaticJoint::Register,
    Wrap_PulleyJoint::Register,
    Wrap_RevoluteJoint::Register,
    Wrap_RopeJoint::Register,
    Wrap_WeldJoint::Register,
    Wrap_WheelJoint::Register,
    nullptr
};
// clang-format on

int Wrap_Physics::Register(lua_State* L)
{
    Physics* instance = instance();
    if (instance == nullptr)
        luax::CatchException(L, [&]() { instance = new Physics(); });
    else
        instance->Retain();

    WrappedModule module {};
    module.instance  = instance;
    module.name      = "physics";
    module.type      = &Module::type;
    module.functions = functions;
    module.types     = types;

    return luax::RegisterModule(L, module);
}
