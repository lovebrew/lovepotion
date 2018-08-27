#include "common/runtime.h"

#include "modules/joystick.h"
#include "objects/gamepad/wrap_gamepad.h"
#include "objects/gamepad/gamepad.h"

bool IR_ENABLED = false;
void Joystick::Initialize()
{
    if (irrstInit() == 0)
        IR_ENABLED = true;

    HIDUSER_EnableGyroscope();
    HIDUSER_EnableAccelerometer();
}

int Joystick::GetJoysticks(lua_State * L)
{
    lua_newtable(L);
    lua_pushnumber(L, 1);
    love_push_userdata(L, DEVICE_INSTANCE);
    lua_settable(L, -3);

    return 1;
}

void Joystick::Exit()
{
    if (IR_ENABLED)
        irrstExit();

    HIDUSER_DisableGyroscope();
    HIDUSER_DisableAccelerometer();
}

int Joystick::Register(lua_State * L)
{
    luaL_Reg reg[] =
    {
        { "getJoysticks", GetJoysticks },
        { 0, 0 }
    };

    luaL_newlib(L, reg);

    return 1;
}