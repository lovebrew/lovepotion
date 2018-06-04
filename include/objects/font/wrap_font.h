#pragma once

int fontNew(lua_State * L);

int fontGetWidth(lua_State * L);

int fontGetHeight(lua_State * L);

int fontToString(lua_State * L);

int fontGC(lua_State * L);

int initFontClass(lua_State * L);