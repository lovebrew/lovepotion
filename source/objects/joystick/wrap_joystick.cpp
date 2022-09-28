#include <objects/joystick/wrap_joystick.hpp>
#include <utilities/guid.hpp>

#include <vector>

#if !defined(__SWITCH__)
std::span<const luaL_Reg> Wrap_Joystick::extension;
#endif

using namespace love;

Joystick<love::Console::Which>* Wrap_Joystick::CheckJoystick(lua_State* L, int index)
{
    return luax::CheckType<Joystick<love::Console::Which>>(L, index);
}

int Wrap_Joystick::IsConnected(lua_State* L)
{
    auto* self = Wrap_Joystick::CheckJoystick(L, 1);

    luax::PushBoolean(L, self->IsConnected());

    return 1;
}

int Wrap_Joystick::GetName(lua_State* L)
{
    auto* self = Wrap_Joystick::CheckJoystick(L, 1);

    lua_pushstring(L, self->GetName().c_str());

    return 1;
}

int Wrap_Joystick::GetID(lua_State* L)
{
    auto* self = Wrap_Joystick::CheckJoystick(L, 1);

    lua_pushinteger(L, self->GetID() + 1);

    int instanceId = self->GetInstanceID();

    if (instanceId >= 0)
        lua_pushinteger(L, instanceId + 1);
    else
        lua_pushnil(L);

    return 2;
}

int Wrap_Joystick::GetGUID(lua_State* L)
{
    auto* self = Wrap_Joystick::CheckJoystick(L, 1);

    luax::PushString(L, self->GetGUID());

    return 1;
}

int Wrap_Joystick::GetDeviceInfo(lua_State* L)
{
    auto* self   = Wrap_Joystick::CheckJoystick(L, 1);
    int vendorId = 0, productId = 0, productVersion = 0;

    self->GetDeviceInfo(vendorId, productId, productVersion);

    lua_pushnumber(L, vendorId);
    lua_pushnumber(L, productId);
    lua_pushnumber(L, productVersion);

    return 3;
}

int Wrap_Joystick::GetAxisCount(lua_State* L)
{
    auto* self = Wrap_Joystick::CheckJoystick(L, 1);

    lua_pushinteger(L, self->GetAxisCount());

    return 1;
}

int Wrap_Joystick::GetButtonCount(lua_State* L)
{
    auto* self = Wrap_Joystick::CheckJoystick(L, 1);

    lua_pushinteger(L, self->GetButtonCount());

    return 1;
}

int Wrap_Joystick::GetHatCount(lua_State* L)
{
    Wrap_Joystick::CheckJoystick(L, 1);

    lua_pushinteger(L, 0);

    return 1;
}

int Wrap_Joystick::GetAxis(lua_State* L)
{
    auto* self = Wrap_Joystick::CheckJoystick(L, 1);
    int axis   = luaL_checkinteger(L, 2) - 1;

    lua_pushnumber(L, self->GetAxis(axis));

    return 1;
}

int Wrap_Joystick::GetAxes(lua_State* L)
{
    auto* self              = Wrap_Joystick::CheckJoystick(L, 1);
    std::vector<float> axes = self->GetAxes();

    for (auto value : axes)
        lua_pushnumber(L, value);

    return (int)axes.size();
}

int Wrap_Joystick::GetHat(lua_State* L)
{
    Wrap_Joystick::CheckJoystick(L, 1);
    luaL_checkinteger(L, 2);

    lua_pushnil(L);

    return 1;
}

int Wrap_Joystick::IsDown(lua_State* L)
{
    auto* self   = Wrap_Joystick::CheckJoystick(L, 1);
    bool isTable = lua_istable(L, 2);

    int count = isTable ? (int)luax::ObjectLength(L, 2) : (lua_gettop(L) - 1);

    if (count == 0)
        luaL_checkinteger(L, 2);

    std::vector<int> buttons;
    buttons.reserve(count);

    if (isTable)
    {
        for (int index = 0; index < count; index++)
        {
            lua_rawgeti(L, 2, index + 1);
            buttons.push_back(luaL_checkinteger(L, -1) - 1);
            lua_pop(L, 1);
        }
    }
    else
    {
        for (int index = 0; index < count; index++)
            buttons.push_back(luaL_checkinteger(L, index + 2) - 1);
    }

    luax::PushBoolean(L, self->IsDown(buttons));

    return 1;
}

int Wrap_Joystick::SetPlayerIndex(lua_State* L)
{
    auto* self = Wrap_Joystick::CheckJoystick(L, 1);
    int index  = luaL_checkinteger(L, 2) - 1;

    self->SetPlayerIndex(index);

    return 0;
}

int Wrap_Joystick::GetPlayerIndex(lua_State* L)
{
    auto* self = Wrap_Joystick::CheckJoystick(L, 1);

    int index = self->GetPlayerIndex();

    lua_pushinteger(L, index >= 0 ? index + 1 : index);

    return 1;
}

int Wrap_Joystick::IsGamepad(lua_State* L)
{
    auto* self = Wrap_Joystick::CheckJoystick(L, 1);

    luax::PushBoolean(L, self->IsGamepad());

    return 1;
}

int Wrap_Joystick::GetGamepadType(lua_State* L)
{
    auto* self = Wrap_Joystick::CheckJoystick(L, 1);

    if (auto found = guid::gamepadTypes.ReverseFind(self->GetGamepadType()))
        lua_pushstring(L, *found);
    else
        lua_pushstring(L, "unknown");

    return 1;
}

int Wrap_Joystick::GetGamepadAxis(lua_State* L)
{
    auto* self       = Wrap_Joystick::CheckJoystick(L, 1);
    const char* name = luaL_checkstring(L, 2);

    if (auto found = Joystick<>::axisTypes.Find(name))
        lua_pushnumber(L, self->GetGamepadAxis(*found));
    else
        return luax::EnumError(L, "gamepad axis", name);

    return 1;
}

int Wrap_Joystick::IsGamepadDown(lua_State* L)
{
    auto* self   = Wrap_Joystick::CheckJoystick(L, 1);
    bool isTable = lua_istable(L, 2);

    int count = isTable ? luax::ObjectLength(L, 2) : (lua_gettop(L) - 1);

    if (count == 0)
        luaL_checkstring(L, 2);

    std::vector<Joystick<>::GamepadButton> buttons;
    buttons.reserve(count);

    if (isTable)
    {
        for (int index = 0; index < count; index++)
        {
            lua_rawgeti(L, 2, index + 1);
            const char* name = luaL_checkstring(L, -1);

            if (auto button = Joystick<>::buttonTypes.Find(name); !button)
                return luax::EnumError(L, "gamepad button", name);
            else
                buttons.push_back(*button);

            lua_pop(L, 1);
        }
    }
    else
    {
        for (int index = 0; index < count; index++)
        {
            const char* name = luaL_checkstring(L, index + 2);

            if (auto button = Joystick<>::buttonTypes.Find(name))
                buttons.push_back(*button);
            else
                return luax::EnumError(L, "gamepad button", name);
        }
    }

    luax::PushBoolean(L, self->IsGamepadDown(buttons));

    return 1;
}

int Wrap_Joystick::IsVibrationSupported(lua_State* L)
{
    auto* self = Wrap_Joystick::CheckJoystick(L, 1);

    luax::PushBoolean(L, self->IsVibrationSupported());

    return 1;
}

int Wrap_Joystick::SetVibration(lua_State* L)
{
    auto* self   = Wrap_Joystick::CheckJoystick(L, 1);
    bool success = false;

    if (lua_isnoneornil(L, 2))
        success = self->SetVibration();
    else
    {
        float left     = luaL_checknumber(L, 2);
        float right    = luaL_optnumber(L, 3, left);
        float duration = luaL_optnumber(L, 4, -1.0f);

        success = self->SetVibration(left, right, duration);
    }

    luax::PushBoolean(L, success);

    return 1;
}

int Wrap_Joystick::GetVibration(lua_State* L)
{
    auto* self = Wrap_Joystick::CheckJoystick(L, 1);

    float left  = 0;
    float right = 0;

    self->GetVibration(left, right);

    lua_pushnumber(L, left);
    lua_pushnumber(L, right);

    return 2;
}

int Wrap_Joystick::GetIndex(lua_State* L)
{
    return 0;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "isConnected",          Wrap_Joystick::IsConnected          },
    { "getName",              Wrap_Joystick::GetName              },
    { "getID",                Wrap_Joystick::GetID                },
    { "getGUID",              Wrap_Joystick::GetGUID              },
    { "getDeviceInfo",        Wrap_Joystick::GetDeviceInfo        },
    { "getAxisCount",         Wrap_Joystick::GetAxisCount         },
    { "getButtonCount",       Wrap_Joystick::GetButtonCount       },
    { "getHatCount",          Wrap_Joystick::GetHatCount          },
    { "getAxis",              Wrap_Joystick::GetAxes              },
    { "getAxes",              Wrap_Joystick::GetAxes              },
    { "getHat",               Wrap_Joystick::GetHat               },
    { "isDown",               Wrap_Joystick::IsDown               },
    { "setPlayerIndex",       Wrap_Joystick::SetPlayerIndex       },
    { "getPlayerIndex",       Wrap_Joystick::GetPlayerIndex       },
    { "isGamepad",            Wrap_Joystick::IsGamepad            },
    { "getGamepadType",       Wrap_Joystick::GetGamepadType       },
    { "getGamepadAxis",       Wrap_Joystick::GetGamepadAxis       },
    { "isGamepadDown",        Wrap_Joystick::IsGamepadDown        },
    { "isVibrationSupported", Wrap_Joystick::IsVibrationSupported },
    { "setVibration",         Wrap_Joystick::SetVibration         },
    { "getVibration",         Wrap_Joystick::GetVibration         },
    { "getConnectedIndex",    Wrap_Joystick::GetIndex             }
};
// clang-format on

int Wrap_Joystick::Register(lua_State* L)
{
    return luax::RegisterType(L, &Joystick<>::type, functions, Wrap_Joystick::extension);
}
