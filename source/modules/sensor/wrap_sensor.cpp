#include <modules/sensor/wrap_sensor.hpp>

using namespace love;

#define instance() (Module::GetInstance<Sensor<Console::Which>>(Module::M_SENSOR))

Sensor<Console::Which>::SensorType Wrap_Sensor::CheckSensorType(lua_State* L, int index)
{
    const char* typeName = luaL_checkstring(L, index);
    std::optional<Sensor<Console::Which>::SensorType> type;

    if (!(type = Sensor<Console::Which>::sensorTypes.Find(typeName)))
        luax::EnumError(L, "sensor mode", Sensor<>::sensorTypes, typeName);

    return *type;
}

// TODO
int Wrap_Sensor::HasSensor(lua_State* L)
{
    return 0;
}

int Wrap_Sensor::IsEnabled(lua_State* L)
{
    auto type = Wrap_Sensor::CheckSensorType(L, 1);

    luax::PushBoolean(L, instance()->IsEnabled(type));

    return 1;
}

int Wrap_Sensor::SetEnabled(lua_State* L)
{
    auto type    = Wrap_Sensor::CheckSensorType(L, 1);
    auto enabled = luax::CheckBoolean(L, 2);

    luax::CatchException(L, [&]() { instance()->SetEnabled(type, enabled); });

    return 0;
}

int Wrap_Sensor::GetData(lua_State* L)
{
    auto type = Wrap_Sensor::CheckSensorType(L, 1);
    std::array<float, 3> data;

    luax::CatchException(L, [&]() { data = instance()->GetData(type); });

    for (float value : data)
        lua_pushnumber(L, value);

    return (int)data.size();
}

int Wrap_Sensor::GetName(lua_State* L)
{
    auto type = Wrap_Sensor::CheckSensorType(L, 1);
    std::string_view name;

    luax::CatchException(L, [&]() { name = instance()->GetSensorName(type); });

    luax::PushString(L, name);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getData",    Wrap_Sensor::GetData    },
    { "getName",    Wrap_Sensor::GetName    },
    { "hasSensor",  Wrap_Sensor::HasSensor  },
    { "isEnabled",  Wrap_Sensor::IsEnabled  },
    { "setEnabled", Wrap_Sensor::SetEnabled }
};
// clang-format on

int Wrap_Sensor::Register(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax::CatchException(L, [&]() { instance = new Sensor<Console::Which>(); });
    else
        instance()->Retain();

    WrappedModule wrappedModule {};
    wrappedModule.instance  = instance;
    wrappedModule.name      = "sensor";
    wrappedModule.type      = &Module::type;
    wrappedModule.functions = functions;
    wrappedModule.types     = nullptr;

    return luax::RegisterModule(L, wrappedModule);
}
