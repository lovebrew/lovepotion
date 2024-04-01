#pragma once

#include "common/luax.hpp"
#include "modules/sensor/Sensor.hpp"

namespace Wrap_Sensor
{
    int hasSensor(lua_State* L);

    int isEnabled(lua_State* L);

    int setEnabled(lua_State* L);

    int getData(lua_State* L);

    int getName(lua_State* L);

    int open(lua_State* L);
} // namespace Wrap_Sensor
