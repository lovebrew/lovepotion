#include <modules/physics/physics.hpp>

#include <common/math.hpp>
#include <common/strongreference.hpp>

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

// #region Body

Body* Physics::NewBody(World* world, float x, float y, Body::Type type) const
{
    return new Body(world, b2Vec2(x, y), type);
}

Body* Physics::NewBody(World* world, Body::Type type) const
{
    return new Body(world, b2Vec2(0, 0), type);
}

Body* Physics::NewCircleBody(World* world, Body::Type type, float x, float y, float radius) const
{
    StrongReference<Body> body(this->NewBody(world, x, y, type), Acquire::NORETAIN);
    StrongReference<CircleShape> shape(this->NewCircleShape(body, 0, 0, radius), Acquire::NORETAIN);

    body->Retain();
    return body.Get();
}

Body* Physics::NewRectangleBody(World* world, Body::Type type, float x, float y, float width,
                                float height, float angle) const
{
    StrongReference<Body> body(this->NewBody(world, x, y, type), Acquire::NORETAIN);
    StrongReference<PolygonShape> shape(this->NewRectangleShape(body, 0, 0, width, height, angle),
                                        Acquire::NORETAIN);

    body->Retain();
    return body.Get();
}

Body* Physics::NewPolygonBody(World* world, Body::Type type, const std::span<Vector2>& points) const
{
    Vector2 origin(0, 0);
    for (int index = 0; index < points.size(); index++)
        origin += points[index] / points.size();

    std::vector<Vector2> localCoords {};
    for (int index = 0; index < points.size(); index++)
        localCoords.push_back(points[index] - origin);

    StrongReference<Body> body(this->NewBody(world, origin.x, origin.y, type), Acquire::NORETAIN);
    StrongReference<PolygonShape> shape(this->NewPolygonShape(body, localCoords),
                                        Acquire::NORETAIN);

    body->Retain();
    return body.Get();
}

Body* Physics::NewEdgeBody(World* world, Body::Type type, float x1, float y1, float x2, float y2,
                           bool oneSided) const
{
    float x = (x2 - x1) / 2.0f;
    float y = (y2 - y1) / 2.0f;

    Vector2 xOrigin(x1 - x, y1 - y);
    Vector2 yOrigin(x2 - x, y2 - y);

    StrongReference<Body> body(this->NewBody(world, x, y, type), Acquire::NORETAIN);
    StrongReference<EdgeShape> shape(
        this->NewEdgeShape(body, xOrigin.x, xOrigin.y, yOrigin.x, yOrigin.y, oneSided),
        Acquire::NORETAIN);

    body->Retain();
    return body.Get();
}

Body* Physics::NewChainBody(World* world, Body::Type type, bool loop,
                            const std::span<Vector2>& points) const
{
    Vector2 origin(0, 0);
    for (int index = 0; index < points.size(); index++)
        origin += points[index] / points.size();

    std::vector<Vector2> localCoords {};
    for (int index = 0; index < points.size(); index++)
        localCoords.push_back(points[index] - origin);

    StrongReference<Body> body(this->NewBody(world, origin.x, origin.y, type), Acquire::NORETAIN);
    StrongReference<ChainShape> shape(this->NewChainShape(body, loop, localCoords),
                                      Acquire::NORETAIN);

    body->Retain();
    return body.Get();
}

// #endregion Body

// #region Shape

CircleShape* Physics::NewCircleShape(Body* body, float x, float y, float radius) const
{
    b2CircleShape shape {};
    shape.m_p      = Physics::ScaleDown(b2Vec2(x, y));
    shape.m_radius = Physics::ScaleDown(radius);

    return new CircleShape(body, shape);
}

PolygonShape* Physics::NewRectangleShape(Body* body, float x, float y, float width, float height,
                                         float angle) const
{
    b2PolygonShape shape {};

    const auto _width  = Physics::ScaleDown(width / 2.0f);
    const auto _height = Physics::ScaleDown(height / 2.0f);
    const auto center  = Physics::ScaleDown(b2Vec2(x, y));

    shape.SetAsBox(_width, _height, center, angle);
    return new PolygonShape(body, shape);
}

EdgeShape* Physics::NewEdgeShape(Body* body, float x1, float y1, float x2, float y2,
                                 bool oneSided) const
{
    b2EdgeShape shape {};
    if (oneSided)
    {
        auto v1 = Physics::ScaleDown(b2Vec2(x1, y1));
        auto v2 = Physics::ScaleDown(b2Vec2(x2, y2));

        shape.SetOneSided(v1, v1, v2, v2);
    }
    else
    {
        auto v1 = Physics::ScaleDown(b2Vec2(x1, y1));
        auto v2 = Physics::ScaleDown(b2Vec2(x2, y2));

        shape.SetTwoSided(v1, v2);
    }

    return new EdgeShape(body, shape);
}

PolygonShape* Physics::NewPolygonShape(Body* body, const std::span<Vector2>& points) const
{
    if (points.size() < 3)
        throw love::Exception("Expected a minimum of 3 vertices, got %d.", points.size());
    else if (points.size() > b2_maxPolygonVertices)
    {
        throw love::Exception("Expected a maximum of %d vertices, got %d.", b2_maxPolygonVertices,
                              points.size());
    }

    b2Vec2 vertices[b2_maxPolygonVertices] {};
    for (int index = 0; index < points.size(); index++)
        vertices[index] = Physics::ScaleDown(b2Vec2(points[index].x, points[index].y));

    b2PolygonShape shape {};
    shape.Set(vertices, points.size());

    return new PolygonShape(body, shape);
}

ChainShape* Physics::NewChainShape(Body* body, bool loop, const std::span<Vector2>& points) const
{
    std::vector<b2Vec2> vertices {};
    for (int index = 0; index < points.size(); index++)
        vertices.push_back(Physics::ScaleDown(b2Vec2(points[index].x, points[index].y)));

    b2ChainShape shape {};

    if (loop)
        shape.CreateLoop(vertices.data(), points.size());
    else
        shape.CreateChain(vertices.data(), points.size(), vertices[0], vertices[points.size() - 1]);

    return new ChainShape(body, shape);
}

// #endregion Shape

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
