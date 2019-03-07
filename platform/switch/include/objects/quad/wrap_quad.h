#pragma once

int quadNew(lua_State * L);

int quadGetViewport(lua_State * L);

int quadSetViewport(lua_State * L);

int quadGetTextureDimensions(lua_State * L);

int quadToString(lua_State * L);

int quadGC(lua_State * L);

int initQuadClass(lua_State * L);