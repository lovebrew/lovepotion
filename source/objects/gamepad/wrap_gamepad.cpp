#include "common/runtime.h"
#include "objects/gamepad/wrap_gamepad.h"

using namespace love;

/*
** @func GetAxes
** @ret float/per axis
*/
int Wrap_Gamepad::GetAxes(lua_State * L)
{
    Gamepad * self = Wrap_Gamepad::CheckJoystick(L, 1);

    size_t axisCount = self->GetAxisCount();

    for (size_t axis = 0; axis < axisCount; axis++)
        lua_pushnumber(L, self->GetAxis(axis + 1));

    return axisCount;
}

/*
** @func GetAxis
** @ret float value for size_t @axis
*/
int Wrap_Gamepad::GetAxis(lua_State * L)
{
    Gamepad * self = Wrap_Gamepad::CheckJoystick(L, 1);

    size_t axis = luaL_checknumber(L, 2);

    lua_pushnumber(L, self->GetAxis(axis));

    return 1;
}

/*
** @func GetButtonCount
** @ret total button count
*/
int Wrap_Gamepad::GetButtonCount(lua_State * L)
{
    Gamepad * self = Wrap_Gamepad::CheckJoystick(L, 1);

    lua_pushnumber(L, self->GetButtonCount());

    return 1;
}

/*
** @func GetGamepadAxis
** @ret float value for string @axis
*/
int Wrap_Gamepad::GetGamepadAxis(lua_State * L)
{
    Gamepad * self = Wrap_Gamepad::CheckJoystick(L, 1);

    std::string axis = luaL_checkstring(L, 2);

    lua_pushnumber(L, self->GetGamepadAxis(axis));

    return 1;
}

/*
** @func GetID
** @ret Joystick ID
*/
int Wrap_Gamepad::GetID(lua_State * L)
{
    Gamepad * self = Wrap_Gamepad::CheckJoystick(L, 1);

    lua_pushnumber(L, self->GetID());

    return 1;
}

/*
** @func GetName
** @ret Joystick Name
*/
int Wrap_Gamepad::GetName(lua_State * L)
{
    Gamepad * self = Wrap_Gamepad::CheckJoystick(L, 1);

    lua_pushstring(L, self->GetName().c_str());

    return 1;
}

/*
** @func GetVibration
** @ret Current vibration for <left, right>
*/
int Wrap_Gamepad::GetVibration(lua_State * L)
{
    Gamepad * self = Wrap_Gamepad::CheckJoystick(L, 1);

    std::pair<float, float> vibrations = self->GetVibration();

    lua_pushnumber(L, vibrations.first);
    lua_pushnumber(L, vibrations.second);

    return 2;
}

/*
** @func IsConnected
** @ret bool Joystick is connected
*/
int Wrap_Gamepad::IsConnected(lua_State * L)
{
    Gamepad * self = Wrap_Gamepad::CheckJoystick(L, 1);

    lua_pushboolean(L, self->IsConnected());

    return 1;
}

/*
** @func IsDown
** @ret bool is index @button down
*/
int Wrap_Gamepad::IsDown(lua_State * L)
{
    Gamepad * self = Wrap_Gamepad::CheckJoystick(L, 1);

    size_t buttonCount = Input::buttons.size();

    size_t index = luaL_checkinteger(L, 2);

    size_t button = std::clamp(index, (size_t)1, buttonCount);

    lua_pushboolean(L, self->IsDown(button));

    return 1;
}

/*
** @func IsGamepad
** @ret bool is it a Gamepad
*/
int Wrap_Gamepad::IsGamepad(lua_State * L)
{
    Gamepad * self = Wrap_Gamepad::CheckJoystick(L, 1);

    lua_pushboolean(L, self->IsGamepad());

    return 1;
}

/*
** @func IsGamepadDown
** @ret bool is string @button down
*/
int Wrap_Gamepad::IsGamepadDown(lua_State * L)
{
    Gamepad * self = Wrap_Gamepad::CheckJoystick(L, 1);

    std::string button = luaL_checkstring(L, 2);

    lua_pushboolean(L, self->IsGamepadDown(button));

    return 1;
}

/*
** @func IsVibrationSupported
** @ret bool if vibration is supported
*/
int Wrap_Gamepad::IsVibrationSupported(lua_State * L)
{
    Gamepad * self = Wrap_Gamepad::CheckJoystick(L, 1);

    lua_pushboolean(L, self->IsVibrationSupported());

    return 1;
}

/*
** @func SetVibration
** Set the vibration value for @left and @right for @duration seconds
** Calling this without values stops all vibration
** @ret bool success
*/
int Wrap_Gamepad::SetVibration(lua_State * L)
{
    Gamepad * self = Wrap_Gamepad::CheckJoystick(L, 1);

    float left = luaL_optnumber(L, 2, 0.0f);
    float right = luaL_optnumber(L, 3, 0.0f);

    float duration = luaL_optnumber(L, 4, 0.0f);

    bool success = self->SetVibration(left, right, duration);

    lua_pushboolean(L, success);

    return 1;
}

/*
** @func CheckJoystick
** Quick wrapper for @Luax::CheckType<T>
*/
Gamepad * Wrap_Gamepad::CheckJoystick(lua_State * L, int index)
{
    return Luax::CheckType<Gamepad>(L, index);
}

int Wrap_Gamepad::Register(lua_State * L)
{
    luaL_Reg reg[] =
    {
        { "getAxes",              GetAxes              },
        { "getAxis",              GetAxis              },
        { "getButtonCount",       GetButtonCount       },
        { "getGamepadAxis",       GetGamepadAxis       },
        { "getID",                GetID                },
        { "getName",              GetName              },
        { "getVibration",         GetVibration         },
        { "isConnected",          IsConnected          },
        { "isDown",               IsDown               },
        { "isGamepad",            IsGamepad            },
        { "isGamepadDown",        IsGamepadDown        },
        { "isVibrationSupported", IsVibrationSupported },
        { "setVibration",         SetVibration         },
        { 0,                      0                    }
    };

    return Luax::RegisterType(L, &Gamepad::type, reg, nullptr);
}
