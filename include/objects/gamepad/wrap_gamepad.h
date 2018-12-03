#pragma once

int gamepadNew(lua_State * L, int id);

int gamepadGetID(lua_State * L);

int gamepadGetAxis(lua_State * L);

int gamepadGetGamepadAxis(lua_State * L);

int gamepadGetButtonCount(lua_State * L);

int gamepadGetName(lua_State * L);

int gamepadSetVibration(lua_State * L);

int gamepadIsVibrationSupported(lua_State * L);

int gamepadIsGamepadDown(lua_State * L);

int gamepadSetLayout(lua_State * L);

int gamepadIsDown(lua_State * L);

int gamepadToString(lua_State * L);

int gamepadGC(lua_State * L);

int initGamepadClass(lua_State * L);