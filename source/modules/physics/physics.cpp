#include <modules/physics/physics.hpp>

#include <common/math.hpp>
#include <objects/body/wrap_body.hpp>

// TODO: remove this
#include <box2d/b2_distance.h>

using namespace love;

float Physics::meter = Physics::DEFAULT_METER;

Physics::Physics() : allocator {}
{
    Physics::meter = DEFAULT_METER;
}

Physics::~Physics()
{}

World* Physics::NewWorld(float gravityX, float gravityY, bool sleep) const
{
    return new World(b2Vec2(gravityX, gravityY), sleep);
}

Body* Physics::NewBody(World* world, float x, float y, Body::Type type) const
{
    return new Body(world, b2Vec2(x, y), type);
}

Body* Physics::NewBody(World* world, Body::Type type) const
{
    return new Body(world, b2Vec2(0, 0), type);
}

void Physics::SetMeter(float scale)
{
    if (scale < 1.0f)
        throw love::Exception("Physics error: invalid meter: %f", scale);

    Physics::meter = scale;
}

float Physics::GetMeter()
{
    return Physics::meter;
}

void Physics::ScaleDown(float& x, float& y)
{
    x /= Physics::meter;
    y /= Physics::meter;
}

void Physics::ScaleUp(float& x, float& y)
{
    x *= Physics::meter;
    y *= Physics::meter;
}

float Physics::ScaleDown(float value)
{
    return value / Physics::meter;
}

float Physics::ScaleUp(float value)
{
    return value * Physics::meter;
}

b2Vec2 Physics::ScaleDown(const b2Vec2& vector)
{
    b2Vec2 result = vector;
    Physics::ScaleDown(result.x, result.y);

    return result;
}

b2Vec2 Physics::ScaleUp(const b2Vec2& vector)
{
    b2Vec2 result = vector;
    Physics::ScaleUp(result.x, result.y);

    return result;
}

b2AABB Physics::ScaleDown(const b2AABB& aabb)
{
    b2AABB result {};
    result.lowerBound = Physics::ScaleDown(aabb.lowerBound);
    result.upperBound = Physics::ScaleDown(aabb.upperBound);

    return result;
}

b2AABB Physics::ScaleUp(const b2AABB& aabb)
{
    b2AABB result {};
    result.lowerBound = Physics::ScaleUp(aabb.lowerBound);
    result.upperBound = Physics::ScaleUp(aabb.upperBound);

    return result;
}

int Physics::GetDistance(lua_State* L)
{
    auto* shapeA = luax::CheckType<Shape>(L, 1);
    auto* shapeB = luax::CheckType<Shape>(L, 2);

    b2DistanceProxy proxyA, proxyB {};
    b2DistanceInput input {};
    b2DistanceOutput output {};
    b2SimplexCache cache {};
    cache.count = 0;

    luax::CatchException(L, [&]() {
        if (!shapeA->IsShapeValid() || !shapeB->IsShapeValid())
            throw love::Exception("The given Shape is not active in the physics World.");

        proxyA.Set(shapeA->fixture->GetShape(), 0);
        proxyB.Set(shapeB->fixture->GetShape(), 0);

        input.proxyA = proxyA;
        input.proxyB = proxyB;

        input.transformA = shapeA->fixture->GetBody()->GetTransform();
        input.transformB = shapeB->fixture->GetBody()->GetTransform();
        input.useRadii   = true;

        b2Distance(&output, &cache, &input);
    });

    lua_pushnumber(L, Physics::ScaleUp(output.distance));
    lua_pushnumber(L, Physics::ScaleUp(output.pointA.x));
    lua_pushnumber(L, Physics::ScaleUp(output.pointA.y));
    lua_pushnumber(L, Physics::ScaleUp(output.pointB.x));
    lua_pushnumber(L, Physics::ScaleUp(output.pointB.y));

    return 5;
}

void Physics::ComputeLinearStiffness(float& stiffness, float& damping, float frequency,
                                     float dampingRatio, const b2Body* bodyA, const b2Body* bodyB)
{
    b2LinearStiffness(stiffness, damping, frequency, dampingRatio, bodyA, bodyB);
}

void Physics::ComputeLinearFrequency(float& frequency, float& dampingRatio, float stiffness,
                                     float damping, b2Body* bodyA, b2Body* bodyB)
{
    const auto massA = bodyA->GetMass();
    const auto massB = bodyB->GetMass();

    float mass = 0.0f;

    if (massA > 0.0f && massB > 0.0f)
        mass = massA * massB / (massA + massB);
    else if (massA > 0.0f)
        mass = massA;
    else
        mass = massB;

    if (mass == 0.0f || stiffness <= 0.0f)
    {
        frequency    = 0.0f;
        dampingRatio = 0.0f;
        return;
    }

    float omega  = b2Sqrt(stiffness / mass);
    frequency    = omega / (2.0f * b2_pi);
    dampingRatio = damping / (mass * 2.0f * omega);
}

void Physics::ComputeAngularStiffness(float& frequency, float& dampingRatio, float stiffness,
                                      float damping, const b2Body* bodyA, const b2Body* bodyB)
{
    b2AngularStiffness(frequency, dampingRatio, stiffness, damping, bodyA, bodyB);
}

void Physics::ComputeAngularFrequency(float& frequency, float& ratio, float stiffness,
                                      float damping, const b2Body* bodyA, const b2Body* bodyB)
{
    const auto inertiaA = bodyA->GetInertia();
    const auto inertiaB = bodyB->GetInertia();

    float inertia = 0.0f;

    if (inertiaA > 0.0f && inertiaB > 0.0f)
        inertia = inertiaA * inertiaB / (inertiaA + inertiaB);
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
