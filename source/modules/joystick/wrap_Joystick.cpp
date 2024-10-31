#include "modules/joystick/wrap_Joystick.hpp"
#include "modules/joystick/wrap_JoystickModule.hpp"

#include "modules/sensor/Sensor.hpp"

using namespace love;

int Wrap_Joystick::isConected(lua_State* L)
{
    auto* self = luax_checkjoystick(L, 1);

    lua_pushboolean(L, self->isConnected());

    return 1;
}

int Wrap_Joystick::getName(lua_State* L)
{
    auto* self = luax_checkjoystick(L, 1);

    luax_pushstring(L, self->getName());

    return 1;
}

int Wrap_Joystick::getID(lua_State* L)
{
    auto* self = luax_checkjoystick(L, 1);

    lua_pushinteger(L, self->getID() + 1);

    int instanceID = self->getInstanceID();

    if (instanceID >= 0)
        lua_pushinteger(L, instanceID + 1);
    else
        lua_pushnil(L);

    return 2;
}

int Wrap_Joystick::getGUID(lua_State* L)
{
    auto* self = luax_checkjoystick(L, 1);

    luax_pushstring(L, self->getGUID());

    return 1;
}

int Wrap_Joystick::getDeviceInfo(lua_State* L)
{
    auto* self = luax_checkjoystick(L, 1);

    int vendor = 0, product = 0, version = 0;
    self->getDeviceInfo(vendor, product, version);

    lua_pushinteger(L, vendor);
    lua_pushinteger(L, product);
    lua_pushinteger(L, version);

    return 3;
}

int Wrap_Joystick::getJoystickType(lua_State* L)
{
    auto* self = luax_checkjoystick(L, 1);

    std::string_view type = "Unknown";
    JoystickBase::getConstant(self->getJoystickType(), type);

    luax_pushstring(L, type);

    return 1;
}

int Wrap_Joystick::getAxisCount(lua_State* L)
{
    auto* self = luax_checkjoystick(L, 1);

    lua_pushinteger(L, self->getAxisCount());

    return 1;
}

int Wrap_Joystick::getButtonCount(lua_State* L)
{
    auto* self = luax_checkjoystick(L, 1);

    lua_pushinteger(L, self->getButtonCount());

    return 1;
}

int Wrap_Joystick::getHatCount(lua_State* L)
{
    auto* self = luax_checkjoystick(L, 1);

    lua_pushinteger(L, self->getHatCount());

    return 1;
}

int Wrap_Joystick::getAxis(lua_State* L)
{
    auto* self = luax_checkjoystick(L, 1);
    int axis   = luaL_checkinteger(L, 2) - 1;

    auto current = (JoystickBase::GamepadAxis)axis;
    lua_pushnumber(L, self->getAxis(current));

    return 1;
}

int Wrap_Joystick::getAxes(lua_State* L)
{
    auto* self = luax_checkjoystick(L, 1);

    std::vector<float> axes = self->getAxes();

    for (float value : axes)
        lua_pushnumber(L, value);

    return (int)axes.size();
}

int Wrap_Joystick::getHat(lua_State* L)
{
    auto* self = luax_checkjoystick(L, 1);
    int hat    = luaL_checkinteger(L, 2) - 1;

    lua_pushnumber(L, 0);

    return 1;
}

int Wrap_Joystick::isDown(lua_State* L)
{
    auto* self = luax_checkjoystick(L, 1);

    bool isTable = lua_istable(L, 2);
    int argc     = isTable ? luax_objlen(L, 2) : lua_gettop(L) - 1;

    if (argc == 0)
        luaL_checkinteger(L, 2);

    std::vector<JoystickBase::GamepadButton> buttons {};
    buttons.reserve(argc);

    if (isTable)
    {
        for (int index = 0; index < argc; index++)
        {
            lua_rawgeti(L, 2, index + 1);

            int button = luaL_checkinteger(L, -1) - 1;
            buttons.push_back(JoystickBase::GamepadButton(button));
            lua_pop(L, 1);
        }
    }
    else
    {
        for (int index = 0; index < argc; index++)
        {
            int button = luaL_checkinteger(L, index + 2) - 1;
            buttons.push_back(JoystickBase::GamepadButton(button));
        }
    }

    luax_pushboolean(L, self->isDown(buttons));

    return 1;
}

int Wrap_Joystick::setPlayerIndex(lua_State* L)
{
    auto* self = luax_checkjoystick(L, 1);
    int index  = luaL_checkinteger(L, 2);

    self->setPlayerIndex(index);

    return 0;
}

int Wrap_Joystick::getPlayerIndex(lua_State* L)
{
    auto* self = luax_checkjoystick(L, 1);

    lua_pushinteger(L, self->getPlayerIndex());

    return 1;
}

int Wrap_Joystick::isGamepad(lua_State* L)
{
    auto* self = luax_checkjoystick(L, 1);

    luax_pushboolean(L, self->isGamepad());

    return 1;
}

int Wrap_Joystick::getGamepadType(lua_State* L)
{
    auto* self = luax_checkjoystick(L, 1);

    std::string_view type = "Unknown";
    JoystickBase::getConstant(self->getGamepadType(), type);

    luax_pushstring(L, type);

    return 1;
}

int Wrap_Joystick::getGamepadAxis(lua_State* L)
{
    auto* self       = luax_checkjoystick(L, 1);
    const char* name = luaL_checkstring(L, 2);

    JoystickBase::GamepadAxis axis;
    if (!JoystickBase::getConstant(name, axis))
        return luax_enumerror(L, "gamepad axis", JoystickBase::GamepadButtons, name);

    lua_pushnumber(L, self->getAxis(axis));

    return 1;
}

int Wrap_Joystick::isGamepadDown(lua_State* L)
{
    auto* self   = luax_checkjoystick(L, 1);
    bool isTable = lua_istable(L, 2);

    int argc = isTable ? luax_objlen(L, 2) : lua_gettop(L) - 1;

    if (argc == 0)
        luaL_checkstring(L, 2);

    std::vector<JoystickBase::GamepadButton> buttons {};
    buttons.reserve(argc);

    JoystickBase::GamepadButton button;

    if (isTable)
    {
        for (int index = 0; index < argc; index++)
        {
            lua_rawgeti(L, 2, index + 1);
            const char* string = luaL_checkstring(L, -1);

            if (!JoystickBase::getConstant(string, button))
                return luax_enumerror(L, "gamepad button", JoystickBase::GamepadButtons, string);

            buttons.push_back(button);
            lua_pop(L, 1);
        }
    }
    else
    {
        for (int index = 0; index < argc; index++)
        {
            const char* string = luaL_checkstring(L, index + 2);

            if (!JoystickBase::getConstant(string, button))
                return luax_enumerror(L, "gamepad button", JoystickBase::GamepadButtons, string);

            buttons.push_back(button);
        }
    }

    luax_pushboolean(L, self->isDown(buttons));

    return 1;
}

int Wrap_Joystick::getGamepadMapping(lua_State* L)
{
    auto* self = luax_checkjoystick(L, 1);

    luax_pushstring(L, "");

    return 1;
}

int Wrap_Joystick::getGamepadMappingString(lua_State* L)
{
    auto* self = luax_checkjoystick(L, 1);

    luax_pushstring(L, "");

    return 1;
}

int Wrap_Joystick::isVibrationSupported(lua_State* L)
{
    auto* self = luax_checkjoystick(L, 1);

    luax_pushboolean(L, self->isVibrationSupported());

    return 1;
}

int Wrap_Joystick::setVibration(lua_State* L)
{
    auto* self = luax_checkjoystick(L, 1);

    bool success = false;

    if (lua_isnoneornil(L, 2))
        success = self->setVibration();
    else
    {
        float left     = luaL_checknumber(L, 2);
        float right    = luaL_optnumber(L, 3, left);
        float duration = luaL_optnumber(L, 4, -1.0f);

        success = self->setVibration(left, right, duration);
    }

    luax_pushboolean(L, success);

    return 1;
}

int Wrap_Joystick::getVibration(lua_State* L)
{
    auto* self = luax_checkjoystick(L, 1);

    float left = 0, right = 0;
    self->getVibration(left, right);

    lua_pushnumber(L, left);
    lua_pushnumber(L, right);

    return 2;
}

int Wrap_Joystick::hasSensor(lua_State* L)
{
    auto* self       = luax_checkjoystick(L, 1);
    const char* name = luaL_checkstring(L, 2);

    auto sensorType = Sensor::SENSOR_MAX_ENUM;
    if (!Sensor::getConstant(name, sensorType))
        return luax_enumerror(L, "sensor type", Sensor::SensorTypes, name);

    luax_pushboolean(L, self->hasSensor(sensorType));

    return 1;
}

int Wrap_Joystick::isSensorEnabled(lua_State* L)
{
    auto* self       = luax_checkjoystick(L, 1);
    const char* name = luaL_checkstring(L, 2);

    auto sensorType = Sensor::SENSOR_MAX_ENUM;
    if (!Sensor::getConstant(name, sensorType))
        return luax_enumerror(L, "sensor type", Sensor::SensorTypes, name);

    luax_pushboolean(L, self->isSensorEnabled(sensorType));

    return 1;
}

int Wrap_Joystick::setSensorEnabled(lua_State* L)
{
    auto* self       = luax_checkjoystick(L, 1);
    const char* name = luaL_checkstring(L, 2);

    auto sensorType = Sensor::SENSOR_MAX_ENUM;
    if (!Sensor::getConstant(name, sensorType))
        return luax_enumerror(L, "sensor type", Sensor::SensorTypes, name);

    bool enabled = luax_checkboolean(L, 3);

    luax_catchexcept(L, [&] { self->setSensorEnabled(sensorType, enabled); });

    return 0;
}

int Wrap_Joystick::getSensorData(lua_State* L)
{
    auto* self       = luax_checkjoystick(L, 1);
    const char* name = luaL_checkstring(L, 2);

    auto sensorType = Sensor::SENSOR_MAX_ENUM;
    if (!Sensor::getConstant(name, sensorType))
        return luax_enumerror(L, "sensor type", Sensor::SensorTypes, name);

    std::vector<float> data {};

    luax_catchexcept(L, [&] { data = self->getSensorData(sensorType); });

    for (float value : data)
        lua_pushnumber(L, value);

    return (int)data.size();
}

// clang-format off
static constexpr luaL_Reg functions[] = {
    { "isConected",              Wrap_Joystick::isConected              },
    { "getName",                 Wrap_Joystick::getName                 },
    { "getID",                   Wrap_Joystick::getID                   },
    { "getGUID",                 Wrap_Joystick::getGUID                 },
    { "getDeviceInfo",           Wrap_Joystick::getDeviceInfo           },
    { "getJoystickType",         Wrap_Joystick::getJoystickType         },
    { "getAxisCount",            Wrap_Joystick::getAxisCount            },
    { "getButtonCount",          Wrap_Joystick::getButtonCount          },
    { "getHatCount",             Wrap_Joystick::getHatCount             },
    { "getAxis",                 Wrap_Joystick::getAxis                 },
    { "getAxes",                 Wrap_Joystick::getAxes                 },
    { "getHat",                  Wrap_Joystick::getHat                  },
    { "isDown",                  Wrap_Joystick::isDown                  },
    { "setPlayerIndex",          Wrap_Joystick::setPlayerIndex          },
    { "getPlayerIndex",          Wrap_Joystick::getPlayerIndex          },
    { "isGamepad",               Wrap_Joystick::isGamepad               },
    { "getGamepadType",          Wrap_Joystick::getGamepadType          },
    { "getGamepadAxis",          Wrap_Joystick::getGamepadAxis          },
    { "isGamepadDown",           Wrap_Joystick::isGamepadDown           },
    // { "getGamepadMapping",       Wrap_Joystick::getGamepadMapping       },
    // { "getGamepadMappingString", Wrap_Joystick::getGamepadMappingString },
    { "isVibrationSupported",    Wrap_Joystick::isVibrationSupported    },
    { "setVibration",            Wrap_Joystick::setVibration            },
    { "getVibration",            Wrap_Joystick::getVibration            },
    { "hasSensor",               Wrap_Joystick::hasSensor               },
    { "isSensorEnabled",         Wrap_Joystick::isSensorEnabled         },
    { "setSensorEnabled",        Wrap_Joystick::setSensorEnabled        },
    { "getSensorData",           Wrap_Joystick::getSensorData           },
    { "getConnectedIndex",       Wrap_JoystickModule::getIndex          }
};
// clang-format on

namespace love
{
    JoystickBase* luax_checkjoystick(lua_State* L, int index)
    {
        return luax_checktype<JoystickBase>(L, index);
    }

    int open_joystick(lua_State* L)
    {
        return luax_register_type(L, &JoystickBase::type, functions);
    }
} // namespace love
