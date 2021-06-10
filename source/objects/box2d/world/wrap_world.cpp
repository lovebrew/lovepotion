#include "world/wrap_world.h"

using namespace love;

World* Wrap_World::CheckWorld(lua_State* L, int index)
{
    World* world = Luax::CheckType<World>(L, index);

    if (!world->IsValid())
        luaL_error(L, "Attempt to use destroyed world.");

    return world;
}

int Register(lua_State* L)
{}
