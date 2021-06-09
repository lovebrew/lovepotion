#include "modules/physics/physics.h"

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

float Physics::ScaleDown(float scale)
{
    return scale / Physics::meter;
}

void Physics::ScaleDown(float& x, float& y)
{
    x /= Physics::meter;
    y /= Physics::meter;
}

b2Vec2 Physics::ScaleDown(const b2Vec2& v)
{
    b2Vec2 t = v;
    Physics::ScaleDown(t.x, t.y);

    return t;
}

/* Scale Up */

float Physics::ScaleUp(float scale)
{
    return scale * Physics::meter;
}

void Physics::ScaleUp(float& x, float& y)
{
    x *= Physics::meter;
    y *= Physics::meter;
}

b2Vec2 Physics::ScaleUp(const b2Vec2& v)
{
    b2Vec2 t = v;
    Physics::ScaleUp(t.x, t.y);

    return t;
}

/* box2d creation methods */
