#pragma once

int fileNew(lua_State * L);

int fileOpen(lua_State * L);

int fileWrite(lua_State * L);

int fileFlush(lua_State * L);

int fileRead(lua_State * L);

int fileGetSize(lua_State * L);

int fileIsOpen(lua_State * L);

int fileGetMode(lua_State * L);

int fileClose(lua_State * L);

int fileGC(lua_State * L);

int initFileClass(lua_State *L);