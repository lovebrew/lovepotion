#pragma once

int textNew(lua_State * L);

int textClear(lua_State * L);

int textGetFont(lua_State * L);

int textGetWidth(lua_State * L);

int textGetHeight(lua_State * L);

int textGetDimensions(lua_State * L);

int textSetFont(lua_State * L);

int textSet(lua_State * L);

int textSetF(lua_State * L);

int textGC(lua_State * L);

int initTextClass(lua_State * L);