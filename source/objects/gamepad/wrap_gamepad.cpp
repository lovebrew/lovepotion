#include "common/runtime.h"

#include "objects/gamepad/gamepad.h"
#include "objects/gamepad/wrap_gamepad.h"

#define CLASS_NAME "Gamepad"
#define CLASS_TYPE LUAOBJ_TYPE_GAMEPAD

vector<Gamepad *> controllers;

int gamepadNew(lua_State * L, int id)
{
    void * raw_self = luaobj_newudata(L, sizeof(Gamepad));

    luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

    Gamepad * self = new (raw_self) Gamepad(id);

    love_register(L, 2, self);

    controllers.push_back(self);

    return 0;
}

//Gamepad:getID
int gamepadGetID(lua_State * L)
{
    Gamepad * self =  (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

    lua_pushinteger(L, self->GetID());

    return 1;
}

//Gamepad:isVibrationSupported
int gamepadIsVibrationSupported(lua_State * L)
{
    Gamepad * self =  (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

    lua_pushboolean(L, self->IsVibrationSupported());

    return 1;
}

//Gamepad:getAxis
int gamepadGetAxis(lua_State * L)
{
    Gamepad * self =  (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

    int axis = luaL_checknumber(L, 2);

    float value = self->GetAxis(axis);

    lua_pushnumber(L, value);

    return 1;
}

int gamepadGetGamepadAxis(lua_State * L)
{
    Gamepad * self = (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

    string axis = luaL_checkstring(L, 2);

    float value = self->GetGamepadAxis(axis);

    lua_pushnumber(L, value);

    return 1;
}

//Gamepad:getButtonCount
int gamepadGetButtonCount(lua_State * L)
{
    Gamepad * self =  (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

    lua_pushinteger(L, self->GetButtonCount());

    return 1;
}

//Gamepad:getName
int gamepadGetName(lua_State * L)
{
    Gamepad * self =  (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

    lua_pushstring(L, self->GetName().c_str());

    return 1;
}

//Gamepad:isGamepadDown
int gamepadIsGamepadDown(lua_State * L)
{
    Gamepad * self =  (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

    string button = luaL_checkstring(L, 2);

    lua_pushboolean(L, self->IsGamepadDown(button));

    return 1;
}

//Gamepad:setLayout
int gamepadSetLayout(lua_State * L)
{
    Gamepad * self =  (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

    string layout = luaL_checkstring(L, 2);
    string holdType = luaL_optstring(L, 3, "default");

    self->SetLayout(layout, holdType);

    return 1;
}

//Gamepad:isDown
int gamepadIsDown(lua_State * L)
{
    Gamepad * self =  (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

    int button = luaL_checkinteger(L, 2);

    lua_pushboolean(L, self->IsDown(button));

    return 1;
}

//Gamepad:setVibration
int gamepadSetVibration(lua_State * L)
{
    Gamepad * self =  (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

    double left = 0;
    double right = 0;

    if (!lua_isnoneornil(L, 2))
    {
        left = luaL_checknumber(L, 2);
        right = luaL_checknumber(L, 3);
    }

    double duration = luaL_optnumber(L, 4, -1);

    self->SetVibration(left, right, duration);

    return 0;
}

int gamepadToString(lua_State * L)
{
    Gamepad * self = (Gamepad *)luaobj_checkudata(L, 1, CLASS_TYPE);

    char * data = self->ToString();

    lua_pushstring(L, data);

    free(data);

    return 1;
}

int gamepadGC(lua_State * L)
{
    return 0;
}

int initGamepadClass(lua_State * L) 
{
    luaL_Reg reg[] = 
    {
        { "__gc",                 gamepadGC                   },
        { "__tostring",           gamepadToString             },
        { "getAxis",              gamepadGetAxis              },
        { "getID",                gamepadGetID                },
        { "getName",              gamepadGetName              },
        { "isDown",               gamepadIsDown               },
        { "isGamepadDown",        gamepadIsGamepadDown        },
        { "isVibrationSupported", gamepadIsVibrationSupported },
        { "setLayout",            gamepadSetLayout            },
        { "setVibration",         gamepadSetVibration         },
        { 0, 0 },
    };

    luaobj_newclass(L, CLASS_NAME, NULL, NULL, reg);

    return 1;
}