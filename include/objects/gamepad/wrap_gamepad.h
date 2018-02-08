#pragma once

int gamepadNew(lua_State * L);

int gamepadNew(lua_State * L, int id);

int gamepadGC(lua_State * L);

int initGamepadClass(lua_State * L);