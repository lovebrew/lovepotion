/** 
 * Copyright (c) 2014 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <compat-5.3.h>
#include "luaobj.h"

object_map map[] = {
  { LUA_TBOOLEAN,            "boolean"        }, 
  { LUA_TFUNCTION,           "function"       }, 
  { LUA_TLIGHTUSERDATA,      "light userdata" }, 
  { LUA_TNIL,                "nil"            },
  { LUA_TNUMBER,             "number"         }, 
  { LUA_TSTRING,             "string"         },
  { LUA_TTABLE,              "table"          }, 
  { LUAOBJ_TYPE_CANVAS,      "Canvas"         },
  { LUAOBJ_TYPE_CHANNEL,     "Channel"        },
  { LUAOBJ_TYPE_FILE,        "File"           },
  { LUAOBJ_TYPE_FILE,        "Font"           }, 
  { LUAOBJ_TYPE_GAMEPAD,     "Gamepad"        }, 
  { LUAOBJ_TYPE_IMAGE,       "Image"          },
  { LUAOBJ_TYPE_IMAGEDATA,   "ImageData"      }, 
  { LUAOBJ_TYPE_QUAD,        "Quad"           }, 
  { LUAOBJ_TYPE_SOURCE,      "Source"         },
  { LUAOBJ_TYPE_SPRITEBATCH, "SpriteBatch"    },
  { LUAOBJ_TYPE_TCP,         "TCP (socket)"   },
  { LUAOBJ_TYPE_TEXT,        "Text"           }, 
  { LUAOBJ_TYPE_THREAD,      "Thread"         }, 
  { LUAOBJ_TYPE_UDP,         "UDP (socket)"   },
  { 0, NULL }
};

int luaobj_newclass(lua_State *L, const char *name, const char *extends, 
                    int (*constructor)(lua_State*), luaL_Reg* reg
) {
  /* Creates and pushes a new metatable which represents a class containing the
   * functions in the `reg` argument. If the `extends` argument is not NULL
   * this class will use the specified class as a super class */

  /* Build metatable */ 
  luaL_newmetatable(L, name);
  /* Add type name string */
  lua_pushstring(L, "__type");
  lua_pushstring(L, name);
  lua_rawset(L, -3);
  /* Make and add func table */
  lua_newtable(L);
  luaL_setfuncs(L, reg, 0); 
  lua_pushstring(L, "__index");
  lua_pushvalue(L, -2);
  lua_rawset(L, -4);
  /* Handle extended */
  if (extends) {
    luaL_getmetatable(L, extends);
    /* Pull metatable functions from base class */
    char *mtfields[] = { "__gc", "__tostring", NULL };
    int i;
    for (i = 0; mtfields[i]; i++) {
      lua_getfield(L, -1, mtfields[i]);
      lua_setfield(L, -4, mtfields[i]);
    }
    lua_setmetatable(L, -2);
  }
  lua_pop(L, 1);              /* Pop func table */
  luaL_setfuncs(L, reg, 0);   /* Set metatable's funcs */
  lua_pop(L, 1);              /* Pop metatable */

  /* Return constructor */
  lua_pushcfunction(L, constructor);

  return 1;
}


void luaobj_setclass(lua_State *L, uint32_t type, const char *name) {
  /* Sets the the metatable of the member at the top of the stack to the
   * class-metatable of the given `name` */
  luaobj_head_t *udata = lua_touserdata(L, -1);
  udata->type = type;
  luaL_getmetatable(L, name);
  if (lua_isnil(L, -1)) {
    luaL_error(L, "missing metatable: assure class '%s' is initialized in love.cpp\n", name);
  }
  lua_setmetatable(L, -2);
}


void *luaobj_newudata(lua_State *L, int size) {
  /* Creates a udata of the requested size, plus space for the luaobj_head_t
   * header. Returns the pointer to the udata's body */
  int s = size + sizeof(luaobj_head_t);
  luaobj_head_t *udata = lua_newuserdata(L, s);
  udata->type = 0;
  return udata + 1;
}


int luaobj_type(lua_State *L, int index, uint32_t type) {
  /* Checks the header of a luaobj udata at the given index, taking into
   * account super-classes it may have extended. Returns the udata's body if
   * it is of the correct class, else errors out */
  index = lua_absindex(L, index);
  luaobj_head_t * udata = lua_touserdata(L, index);

  if (!udata || !(udata->type & type))
    return 0;

  return 1;
}

const char * luax_stype(lua_State * L, int index, uint32_t * utype)
{
    int type = 0;
    if (utype == NULL)
        type = lua_type(L, index);

    char * obj_type = NULL;

    for (uint i = 0; map[i].name; i++)
    {
        if (type == LUA_TUSERDATA)
        {
            luaobj_head_t * udata = lua_touserdata(L, index);
            type = udata->type;
        }
            
        if (utype == NULL)
        {
            if (map[i].type == type)
            {
                obj_type = map[i].name;
                break;
            }
        }
        else
        {
            if (map[i].type == *utype)
            {
                obj_type = map[i].name;
                break;
            }
        }
    }

    return obj_type;
}

void *luaobj_checkudata(lua_State *L, int index, uint32_t type) {
  /* Checks the header of a luaobj udata at the given index, taking into
   * account super-classes it may have extended. Returns the udata's body if
   * it is of the correct class, else errors out */
  index = lua_absindex(L, index);
  luaobj_head_t *udata = lua_touserdata(L, index);
  
  const char * obj_type = "";
  if (!udata || !(udata->type & type))
      obj_type = luax_stype(L, index, &udata->type);
  
  if (strlen(obj_type) != 0)
      luaL_error(L, "bad argument #%d, %s expected, got %s.", index, luax_stype(L, index, &type), obj_type);

  return udata + 1;
}
