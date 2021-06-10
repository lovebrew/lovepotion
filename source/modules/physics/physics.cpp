#include "modules/physics/physics.h"
#include <box2d/b2_distance.h>

#include "fixture.h"
#include "shape.h"

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

/* lua methods */

Body* Physics::NewBody(World* world, float x, float y, Body::Type type)
{
    return new Body(world, b2Vec2(x, y), type);
}

Body* Physics::NewBody(World* world, Body::Type type)
{
    return new Body(world, b2Vec2(0, 0), type);
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
