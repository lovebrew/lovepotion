#pragma once

int threadNew(lua_State * L);

int threadStart(lua_State * L);

int threadStop(lua_State * L);

int threadWait(lua_State * L);

int initThreadClass(lua_State * L);