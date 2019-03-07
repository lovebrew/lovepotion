#pragma once

int imageNew(lua_State * L);

int imageGetWidth(lua_State * L);

int imageGetHeight(lua_State * L);

int imageGetDimensions(lua_State * L);

int imageSetFilter(lua_State * L);

int imageToString(lua_State * L);

int imageGC(lua_State * L);

int imageToString(lua_State * L);

int initImageClass(lua_State * L);