#include "callbacks/querycallback.h"
#include "world/world.h"

#include "common/exception.h"
#include "fixture/fixture.h"

using namespace love;

QueryCallback::QueryCallback(World* world, lua_State* L, int index) :
    world(world),
    L(L),
    functionIndex(index)
{}

QueryCallback::~QueryCallback()
{}

bool QueryCallback::ReportFixture(b2Fixture* fixture)
{
    if (L != nullptr)
    {
        lua_pushvalue(L, this->functionIndex);
        Fixture* f = (Fixture*)world->FindObject(fixture);

        if (!f)
            throw love::Exception("A fixture has escaped Memoizer!");

        Luax::PushType(L, f);
        lua_call(L, 1, 1);

        bool cont = Luax::CheckBoolean(L, -1);
        lua_pop(L, 1);

        return cont;
    }

    return true;
}
