#include "modules/physics/physics.h"
#include <box2d/b2_distance.h>

#include "fixture/fixture.h"
#include "shape/shape.h"
#include "world/world.h"

using namespace love;

/* Apparently LÖVE wants to make this not static */
float Physics::meter = Physics::DEFAULT_METER;

Physics::Physics()
{
    Physics::meter = Physics::DEFAULT_METER;
}

Physics::~Physics()
{}

void Physics::SetMeter(float scale)
{
    if (scale < 1.0f)
        throw love::Exception("Physics error: invalid meter. Scale must be > 1.");

    Physics::meter = scale;
}

float Physics::GetMeter()
{
    return Physics::meter;
}

/* Scale Down */

void Physics::ScaleDown(float& x, float& y)
{
    x /= Physics::meter;
    y /= Physics::meter;
}

float Physics::ScaleDown(float scale)
{
    return scale / Physics::meter;
}

b2Vec2 Physics::ScaleDown(const b2Vec2& v)
{
    b2Vec2 t = v;
    Physics::ScaleDown(t.x, t.y);

    return t;
}

b2AABB Physics::ScaleDown(const b2AABB& aabb)
{
    b2AABB t;

    t.lowerBound = Physics::ScaleDown(aabb.lowerBound);
    t.upperBound = Physics::ScaleDown(aabb.upperBound);

    return t;
}

/* Scale Up */

void Physics::ScaleUp(float& x, float& y)
{
    x *= Physics::meter;
    y *= Physics::meter;
}

float Physics::ScaleUp(float scale)
{
    return scale * Physics::meter;
}

b2Vec2 Physics::ScaleUp(const b2Vec2& v)
{
    b2Vec2 t = v;
    Physics::ScaleUp(t.x, t.y);

    return t;
}

b2AABB Physics::ScaleUp(const b2AABB& aabb)
{
    b2AABB t;

    t.lowerBound = Physics::ScaleUp(aabb.lowerBound);
    t.upperBound = Physics::ScaleUp(aabb.upperBound);

    return t;
}

void Physics::b2LinearFrequency(float& frequency, float& ratio, float stiffness, float damping,
                                b2Body* bodyA, b2Body* bodyB)
{
    float massA = bodyA->GetMass();
    float massB = bodyB->GetMass();

    float mass;

    if (massA > 0.0f && massB > 0.0f)
        mass = massA * massB / (massA / massB);
    else if (massA > 0.0f)
        mass = massA;
    else
        mass = massB;

    if (mass == 0.0f || stiffness <= 0.0f)
    {
        frequency = 0.0f;
        ratio     = 0.0f;

        return;
    }

    float omega = b2Sqrt(stiffness / mass);
    frequency   = omega / (2.0f * b2_pi);
    ratio       = damping / (mass * 2.0f * omega);
}

void Physics::b2AngularFrequency(float& frequency, float& ratio, float stiffness, float damping,
                                 b2Body* bodyA, b2Body* bodyB)
{
    float inertiaA = bodyA->GetInertia();
    float inertiaB = bodyB->GetInertia();

    float inertia;

    if (inertiaA > 0.0f && inertiaB > 0.0f)
        inertia = inertiaA * inertiaB / (inertiaA * inertiaB);
    else if (inertiaA > 0.0f)
        inertia = inertiaA;
    else
        inertia = inertiaB;

    if (inertia == 0.0f || stiffness <= 0.0f)
    {
        frequency = 0.0f;
        ratio     = 0.0f;

        return;
    }

    float omega = b2Sqrt(stiffness / inertia);
    frequency   = omega / (2.0f * b2_pi);
    ratio       = damping / (inertia * 2.0f * omega);
}

/* lua methods */

World* Physics::NewWorld(float gx, float gy, bool sleep)
{
    return new World(b2Vec2(gx, gy), sleep);
}

Body* Physics::NewBody(World* world, float x, float y, Body::Type type)
{
    return new Body(world, b2Vec2(x, y), type);
}

Body* Physics::NewBody(World* world, Body::Type type)
{
    return new Body(world, b2Vec2(0, 0), type);
}

CircleShape* Physics::NewCircleShape(float radius)
{
    return this->NewCircleShape(0, 0, radius);
}

CircleShape* Physics::NewCircleShape(float x, float y, float radius)
{
    b2CircleShape* shape = new b2CircleShape();

    shape->m_p      = Physics::ScaleDown(b2Vec2(x, y));
    shape->m_radius = Physics::ScaleDown(radius);

    return new CircleShape(shape);
}

PolygonShape* Physics::NewRectangleShape(float width, float height)
{
    return this->NewRectangleShape(0, 0, width, height, 0);
}

PolygonShape* Physics::NewRectangleShape(float x, float y, float width, float height, float angle)
{
    b2PolygonShape* polygonShape = new b2PolygonShape();

    float halfWidth  = Physics::ScaleDown(width / 2.0f);
    float halfHeight = Physics::ScaleDown(height / 2.0f);

    b2Vec2 center = Physics::ScaleDown(b2Vec2(x, y));

    polygonShape->SetAsBox(halfWidth, halfHeight, center, angle);

    return new PolygonShape(polygonShape);
}

EdgeShape* Physics::NewEdgeShape(float x1, float y1, float x2, float y2, bool oneSided)
{
    b2EdgeShape* edgeShape = new b2EdgeShape();

    if (oneSided)
    {
        b2Vec2 vec1 = Physics::ScaleDown(b2Vec2(x1, y1));
        b2Vec2 vec2 = Physics::ScaleDown(b2Vec2(x2, y2));

        edgeShape->SetOneSided(vec1, vec1, vec2, vec2);
    }
    else
        edgeShape->SetTwoSided(Physics::ScaleDown(b2Vec2(x1, y1)),
                               Physics::ScaleDown(b2Vec2(x2, y2)));

    return new EdgeShape(edgeShape);
}

int Physics::NewPolygonShape(lua_State* L)
{
    int argc     = lua_gettop(L);
    bool isTable = lua_istable(L, 1);

    if (isTable)
        argc = lua_objlen(L, 1);

    if ((argc % 2) != 0)
        return luaL_error(L, "Number of vertex components must not be a multiple of two.");

    int vertexCount = argc / 2;

    if (vertexCount < 3)
        return luaL_error(L, "Expected a minimum of 3 vertices, got %d.", vertexCount);
    else if (vertexCount > b2_maxPolygonVertices)
        return luaL_error(L, "Expected a maximum of %d vertices, got %d.", b2_maxPolygonVertices,
                          vertexCount);

    b2Vec2 vectors[b2_maxPolygonVertices];

    if (isTable)
    {
        for (int index = 0; index < vertexCount; index++)
        {
            lua_rawgeti(L, 1, 1 + index * 2);
            lua_rawgeti(L, 1, 2 + index * 2);

            float x = luaL_checknumber(L, -2);
            float y = luaL_checknumber(L, -1);

            vectors[index] = Physics::ScaleDown(b2Vec2(x, y));

            lua_pop(L, 2);
        }
    }
    else
    {
        for (int index = 0; index < vertexCount; index++)
        {
            float x = luaL_checknumber(L, 1 + index * 2);
            float y = luaL_checknumber(L, 2 + index * 2);

            vectors[index] = Physics::ScaleDown(b2Vec2(x, y));
        }
    }

    b2PolygonShape* polygonShape = new b2PolygonShape();

    try
    {
        polygonShape->Set(vectors, vertexCount);
    }
    catch (love::Exception&)
    {
        delete polygonShape;
        throw;
    }

    PolygonShape* p = new PolygonShape(polygonShape);

    Luax::PushType(L, p);
    p->Release();

    return 1;
}

int Physics::NewChainShape(lua_State* L)
{
    int argc     = lua_gettop(L) - 1;
    bool isTable = lua_istable(L, 2);

    if (isTable)
        argc = lua_objlen(L, 2);

    if (argc == 0 || ((argc % 2) != 0))
        return luaL_error(L, "Number of vertex components must be a multiple of two.");

    int vertexCount = argc / 2;
    bool isLooping  = lua_toboolean(L, 1);

    b2Vec2* vectors = new b2Vec2[vertexCount];

    if (isTable)
    {
        for (int index = 0; index < vertexCount; index++)
        {
            lua_rawgeti(L, 2, 1 + index * 2);
            lua_rawgeti(L, 2, 2 + index * 2);

            float x = lua_tonumber(L, -2);
            float y = lua_tonumber(L, -1);

            vectors[index] = Physics::ScaleDown(b2Vec2(x, y));

            lua_pop(L, 2);
        }
    }
    else
    {
        for (int index = 0; index < vertexCount; index++)
        {
            float x = luaL_checknumber(L, 2 + index * 2);
            float y = luaL_checknumber(L, 3 + index * 2);

            vectors[index] = Physics::ScaleDown(b2Vec2(x, y));
        }
    }

    b2ChainShape* chainShape = new b2ChainShape();

    try
    {
        if (isLooping)
            chainShape->CreateLoop(vectors, vertexCount);
        else
            chainShape->CreateChain(vectors, vertexCount, vectors[0], vectors[vertexCount - 1]);
    }
    catch (love::Exception&)
    {
        delete[] vectors;
        delete chainShape;
        throw;
    }

    delete[] vectors;

    ChainShape* c = new ChainShape(chainShape);

    Luax::PushType(L, c);
    c->Release();

    return 1;
}

DistanceJoint* Physics::NewDistanceJoint(Body* a, Body* b, float x1, float y1, float x2, float y2,
                                         bool collideConnected)
{
    return new DistanceJoint(a, b, x1, y1, x2, y2, collideConnected);
}

MouseJoint* Physics::NewMouseJoint(Body* body, float x, float y)
{
    return new MouseJoint(body, x, y);
}

RevoluteJoint* Physics::NewRevoluteJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                                         bool collideConnected)
{
    return new RevoluteJoint(a, b, xA, yA, xB, yB, collideConnected);
}

RevoluteJoint* Physics::NewRevoluteJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                                         bool collideConnected, float referenceAngle)
{
    return new RevoluteJoint(a, b, xA, yA, xB, yB, collideConnected, referenceAngle);
}

PrismaticJoint* Physics::NewPrismaticJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                                           float ax, float ay, bool collideConnected)
{
    return new PrismaticJoint(a, b, xA, yA, xB, yB, ax, ay, collideConnected);
}

PrismaticJoint* Physics::NewPrismaticJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                                           float ax, float ay, bool collideConnected,
                                           float referenceAngle)
{
    return new PrismaticJoint(a, b, xA, yA, xB, yB, ax, ay, collideConnected, referenceAngle);
}

PulleyJoint* Physics::NewPulleyJoint(Body* a, Body* b, b2Vec2 groundAnchor1, b2Vec2 groundAnchor2,
                                     b2Vec2 anchor1, b2Vec2 anchor2, float ratio,
                                     bool collideConnected)
{
    return new PulleyJoint(a, b, groundAnchor1, groundAnchor2, anchor1, anchor2, ratio,
                           collideConnected);
}

GearJoint* Physics::NewGearJoint(Joint* a, Joint* b, float ratio, bool collideConnected)
{
    return new GearJoint(a, b, ratio, collideConnected);
}

FrictionJoint* Physics::NewFrictionJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                                         bool collideConnected)
{
    return new FrictionJoint(a, b, xA, yA, xB, yB, collideConnected);
}

WeldJoint* Physics::NewWeldJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                                 bool collideConnected)
{
    return new WeldJoint(a, b, xA, yA, xB, yB, collideConnected);
}

WeldJoint* Physics::NewWeldJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                                 bool collideConnected, float referenceAngle)
{
    return new WeldJoint(a, b, xA, yA, xB, yB, collideConnected, referenceAngle);
}

WheelJoint* Physics::NewWheelJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                                   float ax, float ay, bool collideConnected)
{
    return new WheelJoint(a, b, xA, yA, xB, yB, ax, ay, collideConnected);
}

RopeJoint* Physics::NewRopeJoint(Body* a, Body* b, float x1, float y1, float x2, float y2,
                                 float maxLength, bool collideConnected)
{
    return new RopeJoint(a, b, x1, y1, x2, y2, maxLength, collideConnected);
}

MotorJoint* Physics::NewMotorJoint(Body* a, Body* b)
{
    return new MotorJoint(a, b);
}

MotorJoint* Physics::NewMotorJoint(Body* a, Body* b, float correctionFactor, bool collideConnected)
{
    return new MotorJoint(a, b, correctionFactor, collideConnected);
}

Fixture* Physics::NewFixture(Body* body, Shape* shape, float density)
{
    return new Fixture(body, shape, density);
}

int Physics::GetDistance(lua_State* L)
{
    Fixture* fixtureA = Luax::CheckType<Fixture>(L, 1);
    Fixture* fixtureB = Luax::CheckType<Fixture>(L, 2);

    b2DistanceProxy pA, pB;
    b2DistanceInput i;
    b2DistanceOutput o;
    b2SimplexCache c;

    c.count = 0;

    Luax::CatchException(L, [&]() {
        pA.Set(fixtureA->fixture->GetShape(), 0);
        pB.Set(fixtureB->fixture->GetShape(), 0);

        i.proxyA = pA;
        i.proxyB = pB;

        i.transformA = fixtureA->fixture->GetBody()->GetTransform();
        i.transformB = fixtureB->fixture->GetBody()->GetTransform();

        i.useRadii = true;

        b2Distance(&o, &c, &i);
    });

    lua_pushnumber(L, Physics::ScaleUp(o.distance));
    lua_pushnumber(L, Physics::ScaleUp(o.pointA.x));
    lua_pushnumber(L, Physics::ScaleUp(o.pointA.y));
    lua_pushnumber(L, Physics::ScaleUp(o.pointB.x));
    lua_pushnumber(L, Physics::ScaleUp(o.pointB.y));

    return 5;
}
