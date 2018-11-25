#pragma once

int canvasNew(lua_State * L);

int canvasGetWidth(lua_State * L);

int canvasGetHeight(lua_State * L);

int canvasGetDimensions(lua_State * L);

int canvasRenderTo(lua_State * L);

int canvasToString(lua_State * L);

int canvasGC(lua_State * L);

int initCanvasClass(lua_State * L);