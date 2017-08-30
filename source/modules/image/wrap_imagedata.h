#pragma once

int initImageDataClass(lua_State * L);

int imageDataNew(lua_State * L);

int imageDataGetWidth(lua_State * L);

int imageDataGetHeight(lua_State * L);

int imageDataSetPixel(lua_State * L);

int imageDataGC(lua_State *  L);