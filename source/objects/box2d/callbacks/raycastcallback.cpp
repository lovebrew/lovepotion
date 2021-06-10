#include "callbacks/raycastcallback.h"

#include "common/exception.h"

#include "fixture/fixture.h"
#include "world/world.h"

#include "modules/physics/physics.h"

using namespace love;

RayCastCallback::RayCastCallback(World* world, lua_State* L, int index) :
    world(world),
    L(L),
    functionIndex(index)
{
    luaL_checktype(L, functionIndex, LUA_TFUNCTION);
}

RayCastCallback::~RayCastCallback()
{}

float RayCastCallback::ReportFixture(b2Fixture* bFixture, const b2Vec2& point, const b2Vec2& normal,
                                     float fraction)
{
    if (L != nullptr)
    {
        lua_pushvalue(L, this->functionIndex);
        Fixture* fixture = (Fixture*)world->FindObject(bFixture);

        if (!fixture)
            throw love::Exception("A fixture has escaped Memoizer!");

        Luax::PushType(L, fixture);

        b2Vec2 scaledPoint = Physics::ScaleUp(point);

        lua_pushnumber(L, scaledPoint.x);
        lua_pushnumber(L, scaledPoint.y);
        lua_pushnumber(L, normal.x);
        lua_pushnumber(L, normal.y);
        lua_pushnumber(L, fraction);
        lua_call(L, 6, 1);

        if (!lua_isnumber(L, -1))
            luaL_error(L, "Raycast callback didn't return a number!");

        float fraction = (float)lua_tonumber(L, -1);

        lua_pop(L, 1);

        return fraction;
    }

    return 0;
}
