/**
 * Copyright (c) 2014 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifndef LUAOBJ_H
#define LUAOBJ_H

#include <stdint.h>

#include "../lua/lua.h"
#include "../lua/lualib.h"
#include "../lua/lauxlib.h"

typedef struct {
  uint32_t type;
} luaobj_head_t;


/* Each mask should consist of its unique bit and the unique bit of all its
 * super classes */

#define LUAOBJ_TYPE_SOC (1 << 0)
#define LUAOBJ_TYPE_UDP ((1 << 1) | LUAOBJ_TYPE_SOC)
#define LUAOBJ_TYPE_TCP ((1 << 2) | LUAOBJ_TYPE_SOC)

int luaobj_newclass(lua_State *L, const char *name, const char *extends,
                    int (*constructor)(lua_State*), luaL_Reg* reg);
void luaobj_setclass(lua_State *L, uint32_t type, const char *name);
void *luaobj_newudata(lua_State *L, int size);
void *luaobj_checkudata(lua_State *L, int index, uint32_t type);

#endif
