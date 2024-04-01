#include "modules/sensor/wrap_Sensor.hpp"

using namespace love;

#define instance() Module::getInstance<Sensor>(Module::M_SENSOR)

static Sensor::SensorType luax_checksensortype(lua_State* L, int index)
{
    const char* name = luaL_checkstring(L, index);
    auto type        = Sensor::SENSOR_MAX_ENUM;

    if (!Sensor::getConstant(name, type))
        luax_enumerror(L, "sensor mode", Sensor::SensorTypes, name);

    return type;
}

int Wrap_Sensor::hasSensor(lua_State* L)
{
    auto type = luax_checksensortype(L, 1);

    luax_pushboolean(L, instance()->hasSensor(type));

    return 1;
}

int Wrap_Sensor::isEnabled(lua_State* L)
{
    luax_pushboolean(L, instance()->isEnabled());

    return 1;
}

int Wrap_Sensor::setEnabled(lua_State* L)
{
    auto type   = luax_checksensortype(L, 1);
    bool enable = luax_toboolean(L, 2);

    luax_catchexcept(L, [&] { instance()->setEnabled(type, enable); });

    return 0;
}

int Wrap_Sensor::getData(lua_State* L)
{
    auto type = luax_checksensortype(L, 1);
    std::vector<float> data;

    luax_catchexcept(L, [&] { data = instance()->getData(type); });

    for (auto value : data)
        lua_pushnumber(L, value);

    return (int)data.size();
}

int Wrap_Sensor::getName(lua_State* L)
{
    auto type             = luax_checksensortype(L, 1);
    std::string_view name = "";

    luax_catchexcept(L, [&] { name = instance()->getSensorName(type); });

    luax_pushstring(L, name);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "hasSensor",  Wrap_Sensor::hasSensor  },
    { "isEnabled",  Wrap_Sensor::isEnabled  },
    { "setEnabled", Wrap_Sensor::setEnabled },
    { "getData",    Wrap_Sensor::getData    },
    { "getName",    Wrap_Sensor::getName    }
};
// clang-format on

int Wrap_Sensor::open(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax_catchexcept(L, [&] { instance = new Sensor(); });
    else
        instance->retain();

    WrappedModule module {};
    module.instance  = instance;
    module.name      = "sensor";
    module.type      = &Module::type;
    module.functions = functions;

    return luax_register_module(L, module);
}
