#include "objects/gamepad/wrap_gamepad.h"

using namespace love;

/*
** @func GetAxes
** @ret float/per axis
*/
int Wrap_Gamepad::GetAxes(lua_State* L)
{
    Gamepad* self = Wrap_Gamepad::CheckJoystick(L, 1);

    size_t axisCount = self->GetAxisCount();

    for (size_t axis = 0; axis < axisCount; axis++)
        lua_pushnumber(L, self->GetAxis(axis + 1));

    return axisCount;
}

/*
** @func GetAxis
** @ret float value for size_t @axis
*/
int Wrap_Gamepad::GetAxis(lua_State* L)
{
    Gamepad* self = Wrap_Gamepad::CheckJoystick(L, 1);

    size_t axis = luaL_checknumber(L, 2);

    lua_pushnumber(L, self->GetAxis(axis));

    return 1;
}

/*
** @func GetButtonCount
** @ret total button count
*/
int Wrap_Gamepad::GetButtonCount(lua_State* L)
{
    Gamepad* self = Wrap_Gamepad::CheckJoystick(L, 1);

    lua_pushnumber(L, self->GetButtonCount());

    return 1;
}

/*
** @func GetGamepadAxis
** @ret float value for string @axis
*/
int Wrap_Gamepad::GetGamepadAxis(lua_State* L)
{
    Gamepad* self = Wrap_Gamepad::CheckJoystick(L, 1);

    const char* str = luaL_checkstring(L, 2);
    Gamepad::GamepadAxis axis;

    if (!Gamepad::GetConstant(str, axis))
        return Luax::EnumError(L, "gamepad axis", str);

    lua_pushnumber(L, self->GetGamepadAxis(axis));

    return 1;
}

/*
** @func GetID
** @ret Joystick ID
*/
int Wrap_Gamepad::GetID(lua_State* L)
{
    Gamepad* self = Wrap_Gamepad::CheckJoystick(L, 1);

    lua_pushnumber(L, self->GetID() + 1);

    return 1;
}

/*
** @func GetName
** @ret Joystick Name
*/
int Wrap_Gamepad::GetName(lua_State* L)
{
    Gamepad* self = Wrap_Gamepad::CheckJoystick(L, 1);

    lua_pushstring(L, self->GetName());

    return 1;
}

/*
** @func GetVibration
** @ret Current vibration for <left, right>
*/
int Wrap_Gamepad::GetVibration(lua_State* L)
{
    Gamepad* self = Wrap_Gamepad::CheckJoystick(L, 1);

    float left, right = 0.0f;
    self->GetVibration(left, right);

    lua_pushnumber(L, left);
    lua_pushnumber(L, right);

    return 2;
}

/*
** @func IsConnected
** @ret bool Joystick is connected
*/
int Wrap_Gamepad::IsConnected(lua_State* L)
{
    Gamepad* self = Wrap_Gamepad::CheckJoystick(L, 1);

    lua_pushboolean(L, self->IsConnected());

    return 1;
}

/*
** @func IsDown
** @ret bool is index @button down
*/
int Wrap_Gamepad::IsDown(lua_State* L)
{
    Gamepad* self = Wrap_Gamepad::CheckJoystick(L, 1);

    bool istable = lua_istable(L, 2);
    int num      = istable ? (int)lua_objlen(L, 2) : (lua_gettop(L) - 1);

    if (num == 0)
        luaL_checkinteger(L, 2);

    std::vector<size_t> buttons;
    buttons.reserve(num);

    if (istable)
    {
        for (int i = 0; i < num; i++)
        {
            lua_rawgeti(L, 2, i + 1);
            buttons.push_back(luaL_checkinteger(L, -1) - 1);
            lua_pop(L, 1);
        }
    }
    else
    {
        for (int i = 0; i < num; i++)
            buttons.push_back(luaL_checkinteger(L, i + 2) - 1);
    }

    lua_pushboolean(L, self->IsDown(buttons));

    return 1;
}

/*
** @func IsGamepad
** @ret bool is it a Gamepad
*/
int Wrap_Gamepad::IsGamepad(lua_State* L)
{
    Gamepad* self = Wrap_Gamepad::CheckJoystick(L, 1);

    lua_pushboolean(L, self->IsGamepad());

    return 1;
}

/*
** @func IsGamepadDown
** @ret bool is string @button down
*/
int Wrap_Gamepad::IsGamepadDown(lua_State* L)
{
    Gamepad* self = Wrap_Gamepad::CheckJoystick(L, 1);

    bool istable = lua_istable(L, 2);
    int num      = istable ? (int)lua_objlen(L, 2) : (lua_gettop(L) - 1);

    if (num == 0)
        luaL_checkstring(L, 2);

    std::vector<Gamepad::GamepadButton> buttons;
    buttons.reserve(num);

    Gamepad::GamepadButton button;

    if (istable)
    {
        for (int i = 0; i < num; i++)
        {
            lua_rawgeti(L, 2, i + 1);
            const char* str = luaL_checkstring(L, -1);

            if (!Gamepad::GetConstant(str, button))
                return Luax::EnumError(L, "gamepad button", str);

            buttons.push_back(button);

            lua_pop(L, 1);
        }
    }
    else
    {
        for (int i = 0; i < num; i++)
        {
            const char* str = luaL_checkstring(L, i + 2);

            if (!Gamepad::GetConstant(str, button))
                return Luax::EnumError(L, "gamepad button", str);

            buttons.push_back(button);
        }
    }

    lua_pushboolean(L, self->IsGamepadDown(buttons));

    return 1;
}

/*
** @func IsVibrationSupported
** @ret bool if vibration is supported
*/
int Wrap_Gamepad::IsVibrationSupported(lua_State* L)
{
    Gamepad* self = Wrap_Gamepad::CheckJoystick(L, 1);

    lua_pushboolean(L, self->IsVibrationSupported());

    return 1;
}

/*
** @func SetVibration
** Set the vibration value for @left and @right for @duration seconds
** Calling this without values stops all vibration
** @ret bool success
*/
int Wrap_Gamepad::SetVibration(lua_State* L)
{
    Gamepad* self = Wrap_Gamepad::CheckJoystick(L, 1);

    bool success = false;

    if (lua_isnoneornil(L, 2))
    {
        // Disable joystick vibration if no argument is given.
        success = self->SetVibration();
    }
    else
    {
        float left     = luaL_checknumber(L, 2);
        float right    = luaL_optnumber(L, 3, left);
        float duration = luaL_optnumber(L, 4, -1.0); // -1 is infinite.

        success = self->SetVibration(left, right, duration);
    }

    lua_pushboolean(L, success);

    return 1;
}

/*
** @func CheckJoystick
** Quick wrapper for @Luax::CheckType<T>
*/
Gamepad* Wrap_Gamepad::CheckJoystick(lua_State* L, int index)
{
    return Luax::CheckType<Gamepad>(L, index);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getAxes",              Wrap_Gamepad::GetAxes              },
    { "getAxis",              Wrap_Gamepad::GetAxis              },
    { "getButtonCount",       Wrap_Gamepad::GetButtonCount       },
    { "getGamepadAxis",       Wrap_Gamepad::GetGamepadAxis       },
    { "getID",                Wrap_Gamepad::GetID                },
    { "getName",              Wrap_Gamepad::GetName              },
    { "getVibration",         Wrap_Gamepad::GetVibration         },
    { "isConnected",          Wrap_Gamepad::IsConnected          },
    { "isDown",               Wrap_Gamepad::IsDown               },
    { "isGamepad",            Wrap_Gamepad::IsGamepad            },
    { "isGamepadDown",        Wrap_Gamepad::IsGamepadDown        },
    { "isVibrationSupported", Wrap_Gamepad::IsVibrationSupported },
    { "setVibration",         Wrap_Gamepad::SetVibration         },
    { 0,                      0                                  }
};
// clang-format on

int Wrap_Gamepad::Register(lua_State* L)
{
    return Luax::RegisterType(L, &Gamepad::type, functions, nullptr);
}
