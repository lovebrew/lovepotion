#pragma once

#include <common/luax.hpp>
#include <modules/sensor/sensor.hpp>

namespace Wrap_Sensor
{
    love::Sensor::SensorType CheckSensorType(lua_State* L, int index);

    int HasSensor(lua_State* L);

    int IsEnabled(lua_State* L);

    int SetEnabled(lua_State* L);

    int GetData(lua_State* L);

    int GetName(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_Sensor
