#pragma once

#include <common/luax.hpp>
#include <modules/physics/physics.hpp>

namespace Wrap_Physics
{
    int NewWorld(lua_State* L);

    int NewBody(lua_State* L);

    int GetDistance(lua_State* L);

    int SetMeter(lua_State* L);

    int GetMeter(lua_State* L);

    int ComputeLinearStiffness(lua_State* L);

    int ComputeLinearFrequency(lua_State* L);

    int ComputeAngularStiffness(lua_State* L);

    int ComputeAngularFrequency(lua_State* L);

    int Register(lua_State* L);
}
